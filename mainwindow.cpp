#include "mainwindow.h"

using namespace std;

int MainWindow::socketStatus = -1;
int MainWindow::mainStatus = -1;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    initData();
    initView();
}

/**
* @brief MainWindow::initData  初始化界面数据
*/
void MainWindow::initData(){
    //初始化FPGA设备
    initFPGA();

    //初始化图片信息列表
    showResData << " " << " " << " " << " ";

    //开启写结果文件线程
    WriteResultFileThread *writeThread = new WriteResultFileThread();
    writeThread->moveToThread(&wThread);
    connect(&wThread, &QThread::finished, writeThread, &QObject::deleteLater);
    qRegisterMetaType<PicInfo>("PicInfo");
    connect(this, SIGNAL(writeResult2File(QString, PicInfo)), writeThread, SLOT(updateParam(QString, PicInfo)));
    connect(this, SIGNAL(closeFile(QString)), writeThread, SLOT(closeResultFile(QString)));
    wThread.start();

    //添加Socket监听
    m_s = new QTcpServer(this);
    m_s->listen(QHostAddress::Any,port);
    connect(m_s, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

    //添加处理数据线程
    /**
    * TcpHelper *tcpHelper = new TcpHelper(m_tcp, fd);
    * tcpHelper->moveToThread(&tcpThread);
    * connect(&tcpThread, &QThread::finished, tcpHelper, &QObject::deleteLater);
    * connect(this, &MainWindow::startDataProcessing, tcpHelper, &TcpHelper::onDataProcessing);
    * //连接处理完毕的回调方法
    * connect(tcpHelper, &TcpHelper::dataAllDone, this, &MainWindow::onDataAllDone);
    * tcpThread.start();
    */

    //初始化超时计数器线程
    OverTimerThread *overTimeThread = new OverTimerThread();
    overTimeThread->moveToThread(&otThread);
    connect(&otThread, &QThread::finished, overTimeThread, &QObject::deleteLater);
    connect(this, &MainWindow::startOverTimer, overTimeThread, &OverTimerThread::onCreateTimer);
    connect(overTimeThread, &OverTimerThread::timeOver, this, &MainWindow::onTimeOver);
    otThread.start();
    

    /** syc 打印主线程id
    * QString LogInfo;
    * LogInfo.sprintf("%p", QThread::currentThread());
    * SDKLog::logE("MainThread: cur thread id = ", LogInfo);
    */
}

/**
* @brief MainWindow::initView  初始化视图以及TCP连接
*/
void MainWindow::initView(){
    ui->setupUi(this);

    QString mIP_adr;

    //设置分辨率，适配屏幕
    setMinimumSize(1024, 600);
    setMaximumSize(1024, 600);
    setWindowTitle("Server");
    setWindowFlags(Qt::FramelessWindowHint);

    //添加ip地址
    for (int i = 0; i < QNetworkInterface().allAddresses().length(); ++i) {
        mIP_adr = QNetworkInterface().allAddresses().at(i).toString();
        if(isIpAdr(mIP_adr))
            ui->ip->addItem(mIP_adr);
    }
    //获取并设置端口
    ui->port->setText(QString::number(port));

    //初始化listview
    ui->mListView->setSpacing(15); //设置每个item之间的间距
    ui->mListView->setEditTriggers(QAbstractItemView::NoEditTriggers); //将listview的item设置为不可编辑
    updateListView(showResData, 0); //将默认数据填入listview

    //初始化各个控件状态
    setStatusBar(SOCKET_NOT_CONNECTED);
    setFrameWidgetStatus(FRAME_NOT_START);
    ui->editParam0->setText("4");
    ui->editParam1->setText("30");

    //连接各个控件槽函数
    connect(ui->btnRstChart, SIGNAL(clicked()), this, SLOT(onBtnResetClicked()));
    connect(ui->btnLineChart, SIGNAL(clicked()), this, SLOT(onBtnLineClicked()));
    connect(ui->btnConfirm, SIGNAL(clicked()), this, SLOT(onBtnConfirmClicked()));

    //初始化折线图
    initLineChart();

    QString curPath = QDir::currentPath();
    SDKLog::log("view initial done! current path = ", curPath);
}

/**
 * @brief MainWindow::initLineChart 初始化折线图
 */
void MainWindow::initLineChart(){
    //添加真实高度折线，设为绿色
    ui->lineChart->addGraph();
    QPen penGreen;
    penGreen.setWidth(2);
    penGreen.setBrush(Qt::green);
    ui->lineChart->graph(0)->setPen(penGreen);
    ui->lineChart->graph(0)->setName("Real Height");

    //添加预测高度折线，设为红色
    ui->lineChart->addGraph();
    ui->lineChart->graph(1)->setPen(QPen(Qt::red));
    ui->lineChart->graph(1)->setName("Predict Height");

    //添加回波丢失散点
    ui->lineChart->addGraph();
    QPen penLost;
    penLost.setWidth(2);
    penLost.setBrush(Qt::blue);
    ui->lineChart->graph(2)->setPen(penLost);
    ui->lineChart->graph(2)->setName("Echo Lost");

    //设置横纵坐标范围
    ui->lineChart->xAxis->setRange(0, 250);
    ui->lineChart->yAxis->setRange(1000, 3000);

    ui->lineChart->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->lineChart->graph(1)->setLineStyle(QCPGraph::lsLine);
    ui->lineChart->graph(2)->setLineStyle(QCPGraph::lsNone);

    ui->lineChart->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));
    ui->lineChart->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));
    ui->lineChart->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 2));

    //设置图例
    ui->lineChart->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignBottom);
    ui->lineChart->legend->setBorderPen(Qt::NoPen);
    ui->lineChart->legend->setBrush(Qt::transparent);
    ui->lineChart->legend->setVisible(true);
    ui->lineChart->xAxis->setLabel("Num");
    ui->lineChart->yAxis->setLabel("Height");

    //初始化缓存变量
    Xvalue = QVector<double>(250);
    RealYvalue = QVector<double>(250);
    EvaYvalue = QVector<double>(250);
}

/**
 * @brief MainWindow::updateLineChart 更新折线图数据，最多存储100个点
 */
void MainWindow::updateLineChart(int evaHeight, int realHeight){

    if(chartPointsNum <= 249){ //未填满100个点
        //设置数据
        Xvalue.append(chartPointsNum);
        RealYvalue.append(realHeight);
        //绘制回波丢失异常点
        if(evaHeight == -1){
            EvaYvalue.append(realHeight);
            ui->lineChart->graph(1)->addData(chartPointsNum, realHeight);
            ui->lineChart->graph(2)->addData(chartPointsNum, realHeight);
        }else{
            ui->lineChart->graph(1)->addData(chartPointsNum, evaHeight);
            EvaYvalue.append(evaHeight);
        }

        /** syc 动态范围折线图
        * if(chartPointsNum <= 0){
        *     ui->lineChart->xAxis->setRange(0, 100);
        *     ui->lineChart->yAxis->setRange(1500, 3000);
        * }
        */
        ui->lineChart->graph(0)->addData(chartPointsNum, realHeight);
        
    }else{
        //清空所有缓存数据
        chartPointsNum = 0;
        Xvalue.clear();
        RealYvalue.clear();
        EvaYvalue.clear();
        ui->lineChart->graph(0)->data().data()->clear();
        ui->lineChart->graph(1)->data().data()->clear();
        ui->lineChart->graph(2)->data().data()->clear();

        //添加最新一个点的数据
        Xvalue.append(chartPointsNum);
        RealYvalue.append(realHeight);
        if(evaHeight == -1){
            EvaYvalue.append(realHeight);
            ui->lineChart->graph(1)->addData(chartPointsNum, realHeight);
            ui->lineChart->graph(2)->addData(chartPointsNum, realHeight);
        }else{
            ui->lineChart->graph(1)->addData(chartPointsNum, evaHeight);
            EvaYvalue.append(evaHeight);
        }
        ui->lineChart->graph(0)->addData(chartPointsNum, realHeight);

        /** syc 动态范围折线图
        * Xvalue.removeFirst();
        * RealYvalue.removeFirst();
        * EvaYvalue.removeFirst();
        * Xvalue.append(chartPointsNum);
        * RealYvalue.append(realHeight);
        * EvaYvalue.append(evaHeight);
        

        * ui->lineChart->graph(0)->setData(Xvalue, RealYvalue);
        * ui->lineChart->graph(1)->setData(Xvalue, EvaYvalue);
        * ui->lineChart->xAxis->setRange(Xvalue.first(), Xvalue.last());
        */
    }
    
    ui->lineChart->replot();
    chartPointsNum++;
}

/**
 * @brief MainWindow::onNewConnection
 */
void MainWindow::onNewConnection(){
    m_tcp = m_s->nextPendingConnection();
    setStatusBar(SOCKET_CONNECTED);
    //添加连接状态监听
    connect(m_tcp, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onMSGError(QAbstractSocket::SocketError)));

    //添加接收数据监听
    connect(m_tcp, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void MainWindow::onReadyRead(){
    int tempTest = m_tcp->bytesAvailable();
    //SDKLog::logD("onReadyRead() byteAvailable = ", tempTest);

    onDataProcessing(m_tcp->read(tempTest));
    //emit startDataProcessing(m_tcp->read(m_tcp->bytesAvailable()));
}

/**
 * @brief MainWindow::onMSGError 监听TCP连接状态
 */
void MainWindow::onMSGError(QAbstractSocket::SocketError){
    QString hostAddress=m_tcp->QAbstractSocket::peerAddress().toString();
    int error = m_tcp->error();
    switch(error){
        case QAbstractSocket::RemoteHostClosedError:
            CToast::ShowText("SOCKET连接断开！", 2000, 0.5, 0.5, this);
            SDKLog::logE("Client disconnected! host address = ", hostAddress);
            //TO-DO等队列中的数据全部处理完成后再读取
            resetData();
            onTimeOver();
            setStatusBar(SOCKET_NOT_CONNECTED);
            break;
        default:
            SDKLog::logE("Socket status abnomal! error code = ", m_tcp->errorString());
            resetData();
            onTimeOver();
            setStatusBar(SOCKET_ABNORMAL);
            break;
    }
}

/************************ Socket Data Processing Start ************************/
/**
 * @brief MainWindow::onDataProcessing socket数据到达处理
 */
void MainWindow::onDataProcessing(QByteArray socketData){
    /** syc 查看数据处理线程id
    * QString LogInfo;
    * LogInfo.sprintf("%p", QThread::currentThread());
    * SDKLog::logE("data processing: cur thread id = ", LogInfo);
    */
    setStatusBar(SOCKET_TRANSPORTING);
    //SDKLog::logD("syc1111 ===== socketData = ", socketData.left(10));
    //SDKLog::logD("syc2222 ===== socketData = ", socketData.size());
    while(socketData.size() > 0)
    {
        /**
        if(isNewPacket == true)
        {
            int headerLeftSize = 20 - mHeader.size();
            if(socketData.size() >=  headerLeftSize)
            {

                mHeader.append(socketData.left(headerLeftSize));
                socketData = socketData.mid(headerLeftSize);
                analysisRequest(mHeader, m_tcp);
                mHeader.clear();
                isNewPacket = false;
            }
            else
            {
                mHeader.append(socketData);
                socketData.clear();
            }
        }
        */
        
        if(isNewPacket == true)
        {
            int headerLeftSize = 4 - mHeader.size();
            //int headerLeftSize = 20 - mHeader.size();
            if(socketData.size() >=  headerLeftSize)
            {
                mHeader.append(socketData.left(headerLeftSize));
                socketData = socketData.mid(headerLeftSize);
                //打印包头，赋值全局变量
                mDataCount = 1;
                clientFlag = RECV_PICS;
                mDataLength = 76814; //原labview字节数
                //mDataLength = 76810;
                //mDataLength = 76800;  //纯净的bin，与硬件调试时使用
                SDKLog::log("recv data type header = ", strtol(mHeader.toStdString().c_str(), 0, 16));
                mHeader.clear();
                isNewPacket = false;
            }
            else
            {
                mHeader.append(socketData);
                socketData.clear();
            }
        }
        
        else
        {
            long dataLeftSize = mDataLength - mData.size();
            if(socketData.size() >=  dataLeftSize)
            {
                mData.append(socketData.left(dataLeftSize));
                socketData = socketData.mid(dataLeftSize);
                recvFiles(mData);
                isNewPacket = true;
            }
            else
            {
                mData.append(socketData);
                socketData.clear();
            }
        }

    }
}

/**
 * @brief MainWindow::analysisRequest 从一次新请求的包头开始解析，分离状态码、长度与文件数据
 * @param mRequest 缓冲区的请求，包括了数据包头与数据
 * @param mTcp socket对象
 * 数据包头格式：
 * [0]:       状态码
 * [1-2]:     表示本次发送的文件个数，以16进制表示，单次文件发送上限为256个
 * [3-10]:    表示本次发送数据的总大小，以16进制表示，可以表示的最大长度为2^32个Byte
 * [11-18]:   8个预留位
 * [19]:      结束符'\0'
 */
void MainWindow::analysisRequest(QByteArray mRequest, QTcpSocket* mTcp){
    //从包头获取状态码
    clientFlag = mRequest.left(1).toUInt();
    SDKLog::log("mRequest header = ", mRequest.left(20));
    SDKLog::log("mRequest, clientFlag = ", clientFlag);

    switch (clientFlag) {
        case GET_STATUS://获取参数
            SDKLog::log("clntRequest: GET_STATUS");
            resetData();
            break;

        case ERROR://处理异常
            SDKLog::log("clntRequest: ERROR");
            resetData();
            break;

        case ERRO_STATUS://状态异常
            SDKLog::log("clntRequest: ERROR status");
            resetData();
            break;

        case GET_RESULT: //获取预测结果
            SDKLog::log("clntRequest: GET_RESULT");
            resetData();
            break;

        case RECV_PICS:
        case RECV_TXT:
            SDKLog::log("clntRequest: RECV_FILE");
            //获取本次传输的文件个数
            mDataCount = strtol(mRequest.mid(1, 2).toStdString().c_str(), 0, 16);
            SDKLog::log("mDataCount = ", mDataCount);

            //获取本次传输数据的总长度
            mDataLength = strtol(mRequest.mid(3, 8).toStdString().c_str(), 0, 16);
            SDKLog::log("mDataLength = ", mDataLength);

            if(mDataCount != 0){
                SDKLog::logLong("mData unit size = ", mDataLength / mDataCount);
            }

            //验证解析的文件长度，并截去包头完成第一次数据拼接
            if(!isLengthValid(mDataLength)){
                SDKLog::log("length is illegal! mDataLength = ", mDataLength);
                SDKLog::log("length is illegal! clientFlag = ", clientFlag);

                resetData();
            }
            else{
                //文件头中长度大于1，表示将要发送数据
                mData = mRequest.mid(20);
                //将下次缓冲区到来时判断为非新请求
                isNewPacket = false;
            }
            break;

        default:
            SDKLog::logE("clntRequest:UNKNOW, flag = ", clientFlag);
            //mTcp->write("UNKNOW status");
            break;
    }

    SDKLog::log("analysisRequest mData size = ", mData.size());
}

/**
* @biref MainWindow::recvFiles 接收文件完成，解析图片原始数据显示到屏幕，并存入缓冲区picMap中
* @param recvData  单次接收到的数据，以QByteArray的形式存储
*/
void MainWindow::recvFiles(QByteArray recvData){
    //按照mDataCount识别接收数据包含多少个长度相等的文件，按长度分离后分别处理
    QByteArray mFileUnit;
    long mFileUnitLen = (mDataCount == 0) ? 0 : recvData.size() / mDataCount;
    for(int i=0; i<mDataCount; i++){
        mFileUnit = recvData.left(mFileUnitLen);
        recvData = recvData.mid(mFileUnitLen);
        PicInfo recvImageInfo = decodeImage(mFileUnit);
        send2FPGA(recvImageInfo);
    }

    /** syc start 写文件存储到本地
    recvFile *subt = new recvFile(flieType, mDataCount, mData);
    subt->start();
    connect(subt, &recvFile::over, this, [=]{
        subt->exit();
        subt->wait();
        subt->deleteLater();
    });
    end **/
    //处理完成一次数据后将状态设置为已连接
    MainWindow::socketStatus = SOCKET_CONNECTED;

    emit startOverTimer();

    //接收完成后重置文件长度
    resetData();
}

/**
 * @brief MainWindow::putDataInBuffer 将图片数据存入EIM的缓冲区
 * @param buff_index 缓冲区的序号，EIM有两个缓冲区
 * @param imageData  图片原始数据，不包括包头
 */
void MainWindow::putDataInBuffer(unsigned char buff_index, PicInfo imageInfo){
    short *pData = GetImageBuff(buff_index);            //获取缓存区的首地址
    QByteArray imageData = imageInfo.image;

    if(pData == NULL || imageData.isEmpty()){
        SDKLog::logE("Put data in buffer failed! pls check paramter! buff_index = ", buff_index);
        SDKLog::logE("And imageData size = ", imageData.size());
        SDKLog::isDebug = 1;
        return;
    }else{
        memcpy(pData, imageData.data(), imageData.size());
        //将图片缓存，不包括结果
        imageCache.push(imageInfo);
    }
}

/**
* @brief MainWindow::send2FPGA 将图片的原始数据发送到FPGA进行计算
* @param mImage 图片完整数据，包括包头、原始数据
*/
void MainWindow::send2FPGA(PicInfo mImage){
    short imageResult[2] = {0, 0};
    if(fd < 0){
        printf("FPGA isn't initialed!\n");
        return;
    }
    if(isFirstSend){    //首次发送仅填充缓冲区，流水线工作
        isFirstSend = false;
        imageBuffIndex = 0;
        printf("put the first image to EIM buffer, and start calculate!\n");
        putDataInBuffer(imageBuffIndex, mImage);
        StartCalculateImageResult(fd, imageBuffIndex);
    }else{
        putDataInBuffer(imageBuffIndex ^ 0x01, mImage);
        resultCode = GetImageResult(fd, imageBuffIndex, &imageResult[0], &imageResult[1]);
        if(resultCode == 0){ //计算成功后处理结果
            SDKLog::log("Get FPGA result successfully! This pic num = ", mImage.num);
            SDKLog::log("This pic's ground truth = ", mImage.realHeight);
            SDKLog::logD("Get result0 = ", imageResult[0]);
            SDKLog::logD("Get result1 = ", imageResult[1]);

            //将数据与结果填入缓存
            PicInfo curPic = imageCache.front();
            imageCache.pop();
            curPic.evaHeight = imageResult[0];
            curPic.confi = du.getConfiStrFromShort(imageResult[1]);
            curPic.overlapRate = du.getOverlapRate(curPic.evaHeight, curPic.realHeight);

            //当结果返回后主动触发槽函数
            emit onFpgaDone(curPic, NOT_LAST_TIME);
        }else if(resultCode == -4){
            SDKLog::logD("Image is null!");
            //将空数据填入缓存
            PicInfo curPic = imageCache.front();
            imageCache.pop();
            curPic.evaHeight = -1;
            curPic.confi = "0 %";

            emit onFpgaDone(curPic, NOT_LAST_TIME);
        }else{
            SDKLog::logD("Get image result failed!, resultCode = ", resultCode);
        }

        //切换缓存区
        imageBuffIndex = imageBuffIndex ^ 0x01;
        StartCalculateImageResult(fd, imageBuffIndex); //启动buff计算
    }
}

/**
 * @brief MainWindow::onTimeOver 用于判断接收最后一张图片后的超时操作（10ms），从FPGA取最后一帧图片的结果
 * 
 */
void MainWindow::onTimeOver(){
    //2000ms后超时，从FPGA取最后一个结果
    SDKLog::logE("Time over! Get last image result from FPGA!");
    setMainStatus(MAIN_IDLE);

    short imageResult[2] = {0, 0};
    int isLast = NOT_LAST_TIME;
    while(!imageCache.empty()){
        PicInfo lastImage = imageCache.front();
        imageCache.pop();
        isLast = (imageCache.empty()) ? LAST_TIME : NOT_LAST_TIME; 

        resultCode = GetImageResult(fd, imageBuffIndex, &imageResult[0], &imageResult[1]);
        if(resultCode == 0){ //计算成功后处理结果
            SDKLog::logD("Get last result successfully!");
            SDKLog::log("Get last result0 = ", imageResult[0]);
            SDKLog::log("Get last result1 = ", imageResult[1]);
    
            //将数据与结果填入缓存map
            lastImage.evaHeight = imageResult[0];
            lastImage.confi = du.getConfiStrFromShort(imageResult[1]);
            lastImage.overlapRate = du.getOverlapRate(lastImage.evaHeight, lastImage.realHeight);
                
            //当结果返回后主动触发槽函数
            emit onFpgaDone(lastImage, isLast);
        }else if(resultCode == -4){
            SDKLog::log("Last image is null!");
            //将空数据填入缓存
            lastImage.evaHeight = -1;
            lastImage.confi = "0 %";
                
            emit onFpgaDone(lastImage, isLast);
        }else{
            SDKLog::logE("Get image result failed!, resultCode = ", resultCode);
        }
    }

}

/**
* @brief MainWindow::onFpgaDone FPGA计算结束信号触发函数，将计算结果与对应的图片显示在屏幕上
* @param isLastTime 0：非最后一次计算
*                   1：最后一次计算，需要统计结果并清除缓存
*/
void MainWindow::onFpgaDone(PicInfo mImage, int isLastTime){
    isFirstSend = (isLastTime) ? true : false;

    //将frame widget设置为显示图片模式
    setFrameWidgetStatus(FRAME_WORKING);
    setStatusBar(SOCKET_CONNECTED);

    /* 更新计算结果 */
    SDKLog::log("onFpgaDone write image = ", mImage.toString());
    du.putTempResult(mImage);
    emit writeResult2File(testCaseNum, mImage);
    updateAll(mImage);

    if(isLastTime){
        //最后一次获取统计结果
        int sampleNum = currentNum - dataUtils::echoLostNum;
        
        QString statisticStr = "Total sample num: " + QString::number(currentNum) + "\n"
                            + "<30m Passed num: " + QString::number(dataUtils::successNum) + "\n"
                            + "Echo success num: " + QString::number(dataUtils::echoSuccessNum) + "\n"
                            + "Echo lost num: " + QString::number(dataUtils::echoLostNum) + "\n"
                            + "<200m Passed num: " + QString::number(dataUtils::twoSuccessNum) + "\n"
                            + "Height recognition rate: " + QString::number(du.getHeightRecogRate(sampleNum)) +"\n"
                            + "Echo recognition rate: " + QString::number(du.getEchoRecogRate(sampleNum)) + "\n"
                            + "RMSE: " + QString::number(du.getRMSE(sampleNum));

        /* 关闭文件、清除缓存复位 */
        emit closeFile(statisticStr);
        //ResetFpga(fd);
        //清理结果缓存
        currentNum = 0;
        du.clearTempCache();
        queue<PicInfo> empty;
        swap(empty, imageCache);
        //清理折线图缓存
        chartPointsNum = 0;
        Xvalue.clear();
        RealYvalue.clear();
        EvaYvalue.clear();
        ui->lineChart->graph(0)->data().data()->clear();
        ui->lineChart->graph(1)->data().data()->clear();
        ui->lineChart->graph(2)->data().data()->clear();
    }
}

/**
* @brief MainWindow::decodeImage 将位图解码，移除信息位，仅返回图片数据，图片包头格式如下（共10个字节）：
*                   [0-1] 图片序号
*                   [2-3] 真实高度
*                   [4]   信噪比snr
*                   [5]   干扰种类jam
*                   [6]   地形terrain
*                   [7]   平台platform
*                   [8-9] 图片包头预留位
* @param imageData 图片的全部数据信息，包括包头和实际数据
* @return PicInfo  图片包头 + 图片数据
*/
PicInfo MainWindow::decodeImage(QByteArray imageData){
    PicInfo picInfo;
    /** syc
    * //获取图片序号
    * QString picNum = QString("%1").arg((unsigned char)imageData.at(0), 2, 16, QLatin1Char('0') );
    * picNum.append(QString("%1").arg((unsigned char)imageData.at(1), 2, 16, QLatin1Char('0') ));
    * picInfo.num = strtol(picNum.toStdString().c_str(), 0, 16);
    * 
    * //获取图片的真实高度
    * QString picHeight = QString("%1").arg((unsigned char)imageData.at(2), 2, 16, QLatin1Char('0') );
    * picHeight.append(QString("%1").arg((unsigned char)imageData.at(3), 2, 16, QLatin1Char('0') ));
    * picInfo.realHeight = strtol(picHeight.toStdString().c_str(), 0, 16);
    * 
    * //获取图片的信噪比SNR
    * QString picSNR = QString("%1").arg((unsigned char)imageData.at(4), 2, 16, QLatin1Char('0') );
    * picInfo.snr = strtol(picSNR.toStdString().c_str(), 0, 16);
    * 
    * //获取图片的干扰种类jam
    * QString picJam = QString("%1").arg((unsigned char)imageData.at(5), 2, 16, QLatin1Char('0') );
    * picInfo.jam = strtol(picJam.toStdString().c_str(), 0, 16);
    * 
    * //获取图片的地形terrain
    * QString picTerrain = QString("%1").arg((unsigned char)imageData.at(6), 2, 16, QLatin1Char('0') );
    * picInfo.terrain = strtol(picTerrain.toStdString().c_str(), 0, 16);
    * 
    * //获取图片的平台platform
    * QString picPlatform = QString("%1").arg((unsigned char)imageData.at(7), 2, 16, QLatin1Char('0') );
    * picInfo.platform = strtol(picPlatform.toStdString().c_str(), 0, 16);
    * 
    * //获取软件预测的高度
    * QString picSoftHeight = QString("%1").arg((unsigned char)imageData.at(8), 2, 16, QLatin1Char('0') );
    * picSoftHeight.append(QString("%1").arg((unsigned char)imageData.at(9), 2, 16, QLatin1Char('0') ));
    * picInfo.softHeight = strtol(picSoftHeight.toStdString().c_str(), 0, 16);
    */

    //获取图片序号
    //picInfo.num = picNum++;
    picInfo.num = 0;

    //手动校准真实高度偏移
    int rHeightOffset = 420;
    //int rHeightOffset = 0;

    //获取图片的真实高度
    QString picHeight = QString("%1").arg((unsigned char)imageData.at(1), 2, 16, QLatin1Char('0') );
    picHeight.append(QString("%1").arg((unsigned char)imageData.at(0), 2, 16, QLatin1Char('0') ));
    picInfo.realHeight = strtol(picHeight.toStdString().c_str(), 0, 16) + rHeightOffset;

    //参数占1个字节的情况
    //获取图片的信噪比SNR
    QString picSNR = QString("%1").arg((unsigned char)imageData.at(2), 2, 16, QLatin1Char('0') );
    picInfo.snr = strtol(picSNR.toStdString().c_str(), 0, 16);
    
    //获取图片的干扰种类jam
    QString picJam = QString("%1").arg((unsigned char)imageData.at(3), 2, 16, QLatin1Char('0') );
    picInfo.jam = strtol(picJam.toStdString().c_str(), 0, 16);
    
    //获取图片的地形terrain
    QString picTerrain = QString("%1").arg((unsigned char)imageData.at(4), 2, 16, QLatin1Char('0') );
    picInfo.terrain = strtol(picTerrain.toStdString().c_str(), 0, 16);
    
    //获取图片的平台platform
    QString picPlatform = QString("%1").arg((unsigned char)imageData.at(5), 2, 16, QLatin1Char('0') );
    picInfo.platform = strtol(picPlatform.toStdString().c_str(), 0, 16);
    

    /** 参数占两个字节
    * //获取图片的信噪比SNR
    * QString picSNR = QString("%1").arg((unsigned char)imageData.at(3), 2, 16, QLatin1Char('0') );
    * picSNR.append(QString("%1").arg((unsigned char)imageData.at(2), 2, 16, QLatin1Char('0') ));
    * picInfo.snr = strtol(picSNR.toStdString().c_str(), 0, 16);
    * 
    * //获取图片的干扰种类jam
    * QString picJam = QString("%1").arg((unsigned char)imageData.at(5), 2, 16, QLatin1Char('0') );
    * picJam.append(QString("%1").arg((unsigned char)imageData.at(4), 2, 16, QLatin1Char('0') ));
    * picInfo.jam = strtol(picJam.toStdString().c_str(), 0, 16);
    * 
    * //获取图片的地形terrain
    * QString picTerrain = QString("%1").arg((unsigned char)imageData.at(7), 2, 16, QLatin1Char('0') );
    * picTerrain.append(QString("%1").arg((unsigned char)imageData.at(6), 2, 16, QLatin1Char('0') ));
    * picInfo.terrain = strtol(picTerrain.toStdString().c_str(), 0, 16);
    * 
    * //获取图片的平台platform
    * QString picPlatform = QString("%1").arg((unsigned char)imageData.at(9), 2, 16, QLatin1Char('0') );
    * picPlatform.append(QString("%1").arg((unsigned char)imageData.at(8), 2, 16, QLatin1Char('0') ));
    * picInfo.platform = strtol(picPlatform.toStdString().c_str(), 0, 16);
    */

    //获取软件预测的高度
    QString picSoftHeight = QString("%1").arg((unsigned char)imageData.at(6), 2, 16, QLatin1Char('0') );
    picSoftHeight.append(QString("%1").arg((unsigned char)imageData.at(7), 2, 16, QLatin1Char('0') ));
    picInfo.softHeight = strtol(picSoftHeight.toStdString().c_str(), 0, 16);

    //获取图片的原始数据，截取包头
    picInfo.image = imageData.mid(14);  //labview的数据头默认为14个字节
    //picInfo.image = imageData.mid(10);  //协议中规定头为10个字节
    //picInfo.image = imageData.mid(2);   //仅加了真实高度头为2个字节
    //picInfo.image = imageData;
    //SDKLog::logD("syc------", picInfo.image.left(6));

    SDKLog::logD("decode image data = ", picInfo.toString());
    return picInfo;
}

/**
 * @brief MainWindow::isLengthValid 判断解析的包头长度是否合法
 * @param mLength 单次传输的包头长度
 * @return 合法返回true，非法返回false
 */
bool MainWindow::isLengthValid(int mLength){
    if(mLength == 0 || (mLength > 1 && mLength < 100)){ //判断接收文件长度是否合法
        return false;
    }
    else if(clientFlag != RECV_PICS && clientFlag != RECV_TXT){ //判断request的flag是否合法
        return false;
    }else{
        return true;
    }
}

/**
 * @brief MainWindow::resetData 重置变量数据
 */
void MainWindow::resetData(){
    mData.clear();
    isNewPacket = true;
    clientFlag = -1;
    mDataLength = 0;
}

/************************ Socket Data Processing End ************************/

/**
 * @brief MainWindow::setLED 设置状态指示灯
 * @param label QLabel控件
 * @param color 颜色
 *              0: 灰色
 *              1: 红色
 *              2: 绿色
 *              3: 黄色
 * @param size  大小
 */
void MainWindow::setLED(QLabel* label, int color, int size)
{
    // 将label中的文字清空
    label->setText("");
    // 先设置矩形大小
    // 如果ui界面设置的label大小比最小宽度和高度小，矩形将被设置为最小宽度和最小高度；
    // 如果ui界面设置的label大小比最小宽度和高度大，矩形将被设置为最大宽度和最大高度；
    QString min_width = QString("min-width: %1px;").arg(size);              // 最小宽度：size
    QString min_height = QString("min-height: %1px;").arg(size);            // 最小高度：size
    QString max_width = QString("max-width: %1px;").arg(size);              // 最小宽度：size
    QString max_height = QString("max-height: %1px;").arg(size);            // 最小高度：size
    // 再设置边界形状及边框
    QString border_radius = QString("border-radius: %1px;").arg(size/2);    // 边框是圆角，半径为size/2
    QString border = QString("border:1px solid black;");                    // 边框为1px黑色
    // 最后设置背景颜色
    QString background = "background-color:";
    switch (color) {
        case 0:
            // 灰色
            background += "rgb(190,190,190)";
            break;
        case 1:
            // 红色
            background += "rgb(255,0,0)";
            break;
        case 2:
            // 绿色
            background += "rgb(0,255,0)";
            break;
        case 3:
            // 黄色
            background += "rgb(255,255,0)";
            break;
        default:
            break;
    }

    const QString SheetStyle = min_width + min_height + max_width + max_height + border_radius + border + background;
    label->setStyleSheet(SheetStyle);
}

/**
 * @brief MainWindow::setMainStatus 设置主窗口状态
 */
void MainWindow::setMainStatus(int mStatus){
    switch(mStatus){
        case MAIN_IDLE:
            if(mainStatus != MAIN_IDLE){
                mainStatus = MAIN_IDLE;
                ui->btnRstChart->setAttribute(Qt::WA_TransparentForMouseEvents, false);
                ui->btnConfirm->setAttribute(Qt::WA_TransparentForMouseEvents, false);
                ui->ip->setAttribute(Qt::WA_TransparentForMouseEvents, false);
                ui->btnLineChart->setAttribute(Qt::WA_TransparentForMouseEvents, false);
                ui->editParam0->setAttribute(Qt::WA_TransparentForMouseEvents, false);
                ui->editParam1->setAttribute(Qt::WA_TransparentForMouseEvents, false);
            }
            break;
        case MAIN_BUSY:
            if(mainStatus != MAIN_BUSY){
                mainStatus = MAIN_BUSY;
                ui->btnRstChart->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                ui->btnConfirm->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                ui->ip->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                ui->btnLineChart->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                ui->editParam0->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                ui->editParam1->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            }
            break;
        default:
            break;
    }
}

/**
 * @brief MainWindow::setStatusBar 设置状态栏状态
 * @param mStatus 主窗口状态码
 *                      SOCKET_NOT_CONNECTED 未连接
 *                      SOCKET_CONNECTED     已连接
 *                      SOCKET_TRANSPORTING  传输中
 *                      SOCKET_ABNORMAL      通信异常
 */
void MainWindow::setStatusBar(int mStatus){
    socketStatus = mStatus;
    switch (mStatus) {
        case SOCKET_NOT_CONNECTED:
            setMainStatus(MAIN_IDLE);
            setLED(ui->statusLed, 0, 16);
            ui->statusLabel->setText(SOCKET_NOT_CONNECTED_STR);
            break;

        case SOCKET_CONNECTED:
            setLED(ui->statusLed, 2, 16);
            ui->statusLabel->setText(SOCKET_CONNECTED_STR);
            break;

        case SOCKET_TRANSPORTING:
            setMainStatus(MAIN_BUSY);
            setLED(ui->statusLed, 2, 16);
            ui->statusLabel->setText(SOCKET_CONNECTED_STR);
            break;

        case SOCKET_ABNORMAL:
            setMainStatus(MAIN_IDLE);
            setLED(ui->statusLed, 1, 16);
            ui->statusLabel->setText(SOCKET_ABNORMAL_STR);
            break;
        default:
            setMainStatus(MAIN_IDLE);
            break;
    }
}

/**
 * @brief MainWindow::setFrameWidgetStatus 设置frameWidget状态
 * @param frameStatus frameWidget状态码
 *                      FRAME_NOT_START 未开始测试
 *                      FRAME_ECHO_LOST 回波丢失
 *                      FRAME_WORKING   正常传输数据
 */
void MainWindow::setFrameWidgetStatus(int frameStatus){
    switch (frameStatus) {
        case FRAME_NOT_START:
            ui->numLabel->setVisible(false);
            ui->picNum->setVisible(false);
            ui->typeLabel->setVisible(false);
            ui->widget->setStatus(FRAME_NOT_START);
            break;
        case FRAME_ECHO_LOST:
            ui->widget->setStatus(FRAME_ECHO_LOST);
            break;
        case FRAME_WORKING:
            ui->numLabel->setVisible(true);
            ui->picNum->setVisible(true);
            ui->typeLabel->setVisible(true);
            ui->widget->setStatus(FRAME_WORKING);
            break;
        default:
            break;
    }
}

/**
 * @brief sendCaseNum 将需要的测试用例编号发送给上位机
 * @param caseNum  测试用例编号
 */
void MainWindow::sendCaseNum(QString caseNum){
    if(socketStatus != SOCKET_CONNECTED){
        SDKLog::log("Send caseNum failed, Socket isn't ready!");
        CToast::ShowText("SOCKET未连接！", 2000, 0.5, 0.5, this);
        return;
    }else{
        SDKLog::log("Start case test, num = ", caseNum);
        m_tcp->write(caseNum.toUtf8());
    }
}

/**
 * @brief MainWindow::getImageTypeStr 获取显示图片类型的字符串
 * @param mImage 图片信息
 */
QString MainWindow::getImageTypeStr(PicInfo mImage){
    QString typeStr;
    //拼接干扰类型
    switch(mImage.jam){
        case 1:
            typeStr.append("梳状谱干扰");
            break;
        case 2:
            typeStr.append("灵巧噪声卷积干扰");
            break;
        case 3:
            typeStr.append("频谱弥散干扰");
            break;
        case 4:
            typeStr.append("间歇采样转发干扰");
            break;
        default:
            //SDKLog::logE("getImageTypeStr jamType error! jam = ", mImage.jam);
            typeStr.append("未知干扰");
            break;
    }

    //拼接信噪比
    typeStr.append(" 信噪比：-");
    typeStr.append(QString::number(mImage.snr));
    typeStr.append("db ");

    //拼接地形
    switch(mImage.terrain){
        case 1:
            typeStr.append("平原地形");
            break;
        case 2:
            typeStr.append("山地地形");
            break;
        case 3:
            typeStr.append("城市地形");
            break;
        case 7:
            typeStr.append("城市地形");
            break;
        default:
            //SDKLog::logE("getImageTypeStr terrain error! terrain = ", mImage.terrain);
            typeStr.append("未知地形");
            break;
    }

    //拼接平台
    typeStr.append(" 平台：");
    typeStr.append(QString::number(mImage.platform));
    return typeStr;
}

/*
* @brief MainWindow::updateAll  根据驱动返回的结果序号显示对应的原始图片
* @param index 图片在picMap中的序号
*           showResData格式：
*                0: 估计高度
*                1: 真实高度
*                2: 置信度
*                3: 均方根误差
*/
void MainWindow::updateAll(PicInfo mImage){
    PicInfo curPic = mImage;
    
    /* 更新图片序号与类型 */
    currentNum++;
    ui->picNum->setText(QString::number(currentNum));
    QString typeStr = getImageTypeStr(curPic);
    ui->typeLabel->setText(typeStr);

    if(!QString::compare(curPic.confi, "0 %")){ //置信度为0，表示回波丢失
        SDKLog::log("Confidence is zero! echo lost!");
        //updateListView(showResData, 1);
    }else{
        /** syc 实时显示每帧图像数据
        * //预计高度
        * showResData.replace(0, QString::number(curPic.evaHeight));
        * //真实高度
        * showResData.replace(1, QString::number(curPic.realHeight));
        * //置信度
        * showResData.replace(2, curPic.confi);
        */
        SDKLog::logD("update currentNum  = ", currentNum);
        //<30m个数
        showResData.replace(0, QString::number(du.getHeightRecogRate(currentNum - dataUtils::echoLostNum), 'g', 4));
        //回波辨识成功个数
        showResData.replace(1, QString::number(du.getEchoRecogRate(currentNum - dataUtils::echoLostNum), 'g', 4));
        //回波丢失率
        showResData.replace(2, QString::number(du.getLostRate(currentNum), 'g', 4));
        //RMSE
        showResData.replace(3, QString::number(du.getRMSE(currentNum - dataUtils::echoLostNum), 'g', 4));
        updateListView(showResData, 0);
    }

    //更新折线图数据
    if(isLineChart){
        updateLineChart(curPic.evaHeight, curPic.realHeight);
    }

    //显示对应的原始图片
    ui->widget->updatePix(mImage);
}


/**
* @brief MainWindow::updateListView 更新ListView数据，
* @param rightData 长度为4的QStringList，分别是：
*                   0 σ<30m数
*                   1 回波辨识成功数
*                   2 回波丢失数
*                   3 RMSE
*/
void MainWindow::updateListView(QStringList rightData, int isLost){
    if(!isLost){
        ui->mListView->setVisible(true);
        ui->echoLostLabel->setVisible(false);
        //RMSE为空时隐藏显示
        int RMSEIndex = (QString::compare(rightData.at(3), "-1") == 0) ? 1 : 0;
        //重新填充item数据
        QStandardItemModel *pModel = new QStandardItemModel();
        for (int i=0; i<LEFT_LABEL.size()-RMSEIndex; ++i) {
            QStandardItem *pItem = new QStandardItem;
            itemData itemData;
            itemData.leftLabel = LEFT_LABEL.at(i);
            itemData.rightData = rightData.at(i);
            pItem->setData(QVariant::fromValue(itemData), Qt::UserRole+1);
            pModel->appendRow(pItem);
        }
    
        ItemDelegate *pItemDelegate = new ItemDelegate(this);
        ui->mListView->setItemDelegate(pItemDelegate);
        ui->mListView->setModel(pModel);
    }else{
        SDKLog::log("updateListView data is null, echo lost!");
        ui->mListView->setVisible(false);
        ui->echoLostLabel->setVisible(true);
    }
    
}

/**
 * @brief getResultData 获取驱动返回的预测结果，暂时写为测试文件
 * @return 将预测结果转为字节数组
 */
QByteArray MainWindow::getResultData(){
    QByteArray mResult;
    QFile relHeight("../qttcpserver/relHeight_JSR2_SNR-10_jammer3_terrain0_0.txt");

    //判断文件是否为空
    if(relHeight.exists()){
        relHeight.open(QIODevice::ReadOnly);
        mResult = relHeight.readAll();
    }else{
        SDKLog::log("Open file failed! checkout your file path!");
        mResult.clear();
    }
    relHeight.close();
    //qDebug() << mResult.data();
    return mResult;
}

/**
 * @brief MainWindow::isIpAdr 判断ip地址是否合法
 * @param ipAdr ip地址
 * @return 返回布尔值
 */
bool MainWindow::isIpAdr(QString ipAdr){
        string m_ip = ipAdr.toStdString();
        if(m_ip == "127.0.0.1"){ //过滤掉本地回环ip
            return false;
        }
        //SDKLog::log(m_ip, m_ip);
        regex pattern("((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
        smatch res;
        return regex_match(m_ip, res, pattern);
}

MainWindow::~MainWindow(){
    ::close(fd);
    emit closeFile("");
    delete ui;
}

/**
* @brief MainWindow::onBtnResetClicked 重置FPGA按钮
*/
void MainWindow::onBtnResetClicked(){
    /** 重置FPGA
    * if(fd < 0){
    *     SDKLog::logE("ResetFpga failed! fd<0");
    * }else{
    *     ResetFpga(fd);
    * }
    */
    if(!isEnhanceImg){
        isEnhanceImg = 1;
        ui->widget->setImgEnhance(1);
        ui->btnRstChart->setText("关闭图像增强");
        CToast::ShowText("图像增强已开启!", 2000, 0.5, 0.5, this);
    }else{
        isEnhanceImg = 0;
        ui->widget->setImgEnhance(0);
        ui->btnRstChart->setText("开启图像增强");
        CToast::ShowText("图像增强已关闭!", 2000, 0.5, 0.5, this);
    }
}

/**
* @brief MainWindow::onBtnLineClicked 开启/关闭 折线图按钮
*/
void MainWindow::onBtnLineClicked(){
    if(!isLineChart){
        isLineChart = 1;
        CToast::ShowText("折线图已开启!", 2000, 0.5, 0.5, this);
        ui->btnLineChart->setText("关闭折线图");
    }else{
        isLineChart = 0;
        //关闭折线图，清除数据
        chartPointsNum = 0;
        Xvalue.clear();
        RealYvalue.clear();
        EvaYvalue.clear();
        ui->lineChart->graph(0)->data().data()->clear();
        ui->lineChart->graph(1)->data().data()->clear();
        ui->lineChart->graph(2)->data().data()->clear();
        ui->lineChart->replot();

        CToast::ShowText("折线图已关闭!", 2000, 0.5, 0.5, this);
        ui->btnLineChart->setText("开启折线图");
    }
    
    /** syc 复位FOGA
    * if(fd < 0){
    *     printf("FPGA reset failed! isn't initialed!\n");
    * }else{
    *     ResetFpga(fd);
    * }
    */
}

/**
* @brief MainWindow::onBtnConfirmClicked 更新参数按钮，param0是子图个数，param1是距离
*/
void MainWindow::onBtnConfirmClicked(){
    if(fd < 0){
        SDKLog::logE("SetFpgaParams failed! fd<0");
    }else{
        short param0 = ui->editParam0->text().toUShort(nullptr, 10);
        short param1 = ui->editParam1->text().toUShort(nullptr, 10);
        if(SetFpgaParams(fd, param0, param1) == 0){
            CToast::ShowText("更新参数成功!", 2000, 0.5, 0.5, this);
        }else{
            CToast::ShowText("更新参数失败!", 2000, 0.5, 0.5, this);
        }
        
        
    }
}

/*
* @brief MainWindow::initFPGA 初始化FPGA设备
*/
void MainWindow::initFPGA(){
    fd = InitEimPort(PIC_WIDTH, PIC_HEIGHT, 100);
    ResetFpga(fd);
    SetFpgaParams(fd, 4, 30);
    if(fd < 0){
        printf("Init Eim port failed!!\n");
        return;
    }
    printf("Init and reset FPGA done!\n");
}

/*************************** OverTimerThread ***************************/

OverTimerThread::OverTimerThread(QObject *parent) : QThread{parent}{
}

OverTimerThread::~OverTimerThread(){
    overTimer->stop();
    //overTimer->deleteLater();
}

void OverTimerThread::onCreateTimer()
{

    overTimer = getTimerInstance();
    overTimer->start(1000);
}

void OverTimerThread::onTimeOver()
{
    /** syc 打印超时线程id
    * QString LogInfo;
    * LogInfo.sprintf("%p", QThread::currentThread());
    * SDKLog::logE("OverTimerThread: cur thread id = ", LogInfo);
    */    
    int count = 10;
    while(count > 0){
        if(MainWindow::socketStatus != SOCKET_CONNECTED){
            break;
        }
        //SDKLog::logE("main status = ", MainWindow::socketStatus);
        count--;
    }
    SDKLog::log("time over, count = ", count);
    if(count == 0){
        emit timeOver();
    }
}

QTimer* OverTimerThread::getTimerInstance(){
    if(overTimer == nullptr){
        overTimer = new QTimer();
        overTimer->setSingleShot(true);
        connect(overTimer, SIGNAL(timeout()), this, SLOT(onTimeOver()));
    }

    return overTimer;
}