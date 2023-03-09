#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QLabel>
#include <QFile>
#include <QAbstractItemView>
#include <QDir>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QStandardItemModel>
#include <regex>
#include <sys/io.h>
#include <iostream>
#include <QTimer>
#include <QThread>
#include <QTextStream>
#include <QMetaType>
#include <QVector>

#include "defines.h"
#include "SDKLog.h"
#include "dataUtils.h"
#include "writeResultFileThread.h"
#include "itemDelegate.h"
#include "ui_mainwindow.h"
#include "eimController.h"

using namespace std;
class QPushButton;
class QLabel;
class QGroupBox;
class QVBoxLayout;
class QRadioButton;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static int socketStatus;
    static int mainStatus;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void startOverTimer();
    void startDataProcessing(QByteArray socketData);
    void writeResult2File(QString mTestCaseNum, PicInfo imageInfo);
    void closeFile(QString resultStatisticStr);

public slots:
    void onFpgaDone(PicInfo mImage, int isLastTime);//FPGA结果返回后需要刷新图片
    void onTimeOver();              //用于执行接受数据超时逻辑，取最后一张图片的结果

private slots:
    void onBtnLineClicked();         //开关折线图点击事件
    void onBtnResetClicked();        //重置FPGA点击事件
    void onBtnConfirmClicked();      //更新参数点击事件
    void onMSGError(QAbstractSocket::SocketError); //监听TCP连接状态
    void onNewConnection();         //socket新连接事件
    void onReadyRead();             //socket数据到达事件，在主线程中接收
    void onDataProcessing(QByteArray socketData);

private:
    //主窗口视图
    Ui::MainWindow *ui;
    QLabel *m_status;
    void initView();
    void initData();

    //折线图绘制
    //折线图数据，最多存储１００条
    int chartPointsNum = 0;
    QVector<double> Xvalue;
    QVector<double> RealYvalue;
    QVector<double> EvaYvalue;
    void initLineChart();
    void updateLineChart(int evaHeight, int realHeight);

    //写文件线程
    QFile *resultFile;
    QThread wThread;
    unsigned int resultFileNum = 0;
    const QString ROOT_PATH = "/forlinx/qtbin/socket/";

    //超时处理线程
    QThread otThread;

    //tcp本地通信协议
    unsigned long currentNum = 0;
    queue<PicInfo> imageCache;
    QTcpServer *m_s;
    QTcpSocket *m_tcp;
    int port = 8889;
    bool isIpAdr(QString ipAdr);
    
    //数据到达处理
    long picNum = 1;
    int clientFlag = -1;
    bool isNewPacket = true;
    long mDataLength = 0;
    long mDataCount = 0;
    QByteArray mData; //拼装数据时的载体
    QByteArray mHeader;
    void analysisRequest(QByteArray mRequest, QTcpSocket* mTcp);
    void recvFiles(QByteArray recvData);
    PicInfo decodeImage(QByteArray imageData);
    bool isLengthValid(int mLength);
    void resetData();

    //驱动数据处理
    int fd = -1;
    int resultCode = -1;
    bool isFirstSend = true;
    unsigned char imageBuffIndex = 0;
    void initFPGA();
    void putDataInBuffer(unsigned char buff_index, PicInfo imageInfo);
    void send2FPGA(PicInfo mImage);

    //工具方法
    dataUtils du;
    QByteArray getResultData();

    //向上位机发送测试用例序号
    QString testCaseNum;
    void sendCaseNum(QString caseNum);
    
    //刷新UI
    int isLineChart = 0;
    int isEnhanceImg = 0;
    const QStringList LEFT_LABEL = {
        STR_30M, STR_ECHO_SUCCESS, STR_ECHO_LOST,
        STR_RMSE,
    };
    QStringList showResData;
    void setLED(QLabel* label, int color, int size);
    void setMainStatus(int mStatus);
    void setStatusBar(int mStatus);
    void setFrameWidgetStatus(int frameStatus);
    QString getImageTypeStr(PicInfo mImage);
    void updateAll(PicInfo mImage);
    void updateListView(QStringList rightData, int isLost);
};

class OverTimerThread: public QThread
{
    Q_OBJECT
public:
    explicit OverTimerThread(QObject *parent = nullptr);
    ~OverTimerThread();
 
private:
    QTimer *overTimer = nullptr;
    dataUtils du;

    //单例QTimer
    QTimer* getTimerInstance();

signals:
    void timeOver();

public slots:
    void onCreateTimer();
    void onTimeOver();              //用于执行接受数据超时逻辑，取最后一张图片的结果

};

#endif // MAINWINDOW_H
