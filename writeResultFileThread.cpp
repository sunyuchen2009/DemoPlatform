#include "writeResultFileThread.h"

/**
 * @brief WriteResultFileThread::WriteResultFileThread 开辟子线程执行写操作
 * @param parent
 */
WriteResultFileThread::WriteResultFileThread(QObject *parent)
    : QThread{parent}
{
    mImageInfo.num = -1;
    mTestCaseNum = "";
    mResultFile = new QFile(ROOT_PATH + "result-" + QString::number(resultFileNum) + ".txt");
    mResultFile->open(QFile::WriteOnly);
}

void WriteResultFileThread::run(){
    if(mImageInfo.num == -1){
        SDKLog::logE("WriteResultFileThread isn't start!");
    }else{
        writeResult2File(mImageInfo);
    }
    exec();
}

/**
 * @brief WriteResultFileThread::writeLost2File 将Lost图片保存到本地
 * @param imageInfo 图片信息
 */
void WriteResultFileThread::writeLost2File(PicInfo imageInfo){
    QString lostPath = lostFolderPath + QString::fromStdString(imageInfo.toString()) + ".bin";
    QFile *lostPic = new QFile(lostPath);
    QByteArray imageData = imageInfo.image;

    lostPic->open(QFile::WriteOnly);
    lostPic->write(imageData);
    lostPic->close();
    lostPic->deleteLater();
}

/**
 * @brief WriteResultFileThread::writeResult2File 将图片信息写入文件
 * @param imageInfo 图片信息
 */
void WriteResultFileThread::writeResult2File(PicInfo imageInfo){
    if(!mResultFile->exists()){
        SDKLog::logE("writeResult2File failed! mFile isn't exists!");
        return;
    }else if(!mResultFile->isOpen()){
        resultFileNum++;
        mResultFile = new QFile(ROOT_PATH + "result-" + QString::number(resultFileNum) + ".txt");
        mResultFile->open(QFile::WriteOnly);
        hundredAccNum = 0;
        hundredLostNum = 0;
        writeResult2File(imageInfo);
        return;
    }else{
        QTextStream txtOutput(mResultFile);
        numTemp++;

        //拼接结果字符串
        QString infoStr = QString::number(numTemp) + " " + QString::fromStdString(imageInfo.toString());

        //拼接测试用例字符串
        QString caseStr = "###250-Result:\n";

        //抓取所有图片
        //writeLost2File(imageInfo);

        if(imageInfo.evaHeight == -1){
            //writeLost2File(imageInfo);
            infoStr.append(" Lost");
            hundredLostNum++;
        }else{
            //统计<30m指标
            if(abs(imageInfo.evaHeight - imageInfo.realHeight) >= 30){
                infoStr.append(" 30m_Unpassed");
            }
            //统计回波辨识成功指标
            if(imageInfo.overlapRate < 0.6){
                infoStr.append(" Not_recognized");
            }else{
                hundredAccNum++;
            }
            //统计与软件高度的差值
            if(imageInfo.softHeight != 0 && imageInfo.softHeight != -1){
                int diff = abs(imageInfo.softHeight - imageInfo.evaHeight);
                softHeightTemp += diff;
                infoStr.append(" Diff:" + QString::number(diff) );
            }
    
            //统计SNR信噪比结果
            switch(imageInfo.snr){
                case 10:
                    if(imageInfo.overlapRate > 0.6){
                        snr10_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        snr10_heightSuccessNum++;
                    }
                    snr10_num = (imageInfo.evaHeight == -1) ? snr10_num : (snr10_num + 1);
                    break;
                case 15:
                    if(imageInfo.overlapRate > 0.6){
                        snr15_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        snr15_heightSuccessNum++;
                    }
                    snr15_num = (imageInfo.evaHeight == -1) ? snr15_num : (snr15_num + 1);
                    break;
                case 20:
                    if(imageInfo.overlapRate > 0.6){
                        snr20_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        snr20_heightSuccessNum++;
                    }
                    snr20_num = (imageInfo.evaHeight == -1) ? snr20_num : (snr20_num + 1);
                    break;
                default:
                    SDKLog::logE("SNR Exception! SNR = ", imageInfo.snr);
                    break;
            }
    
            //统计地形结果
            switch(imageInfo.terrain){
                case 1:
                    if(imageInfo.overlapRate > 0.6){
                        terrain1_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        terrain1_heightSuccessNum++;
                    }
                    terrain1_num = (imageInfo.evaHeight == -1) ? terrain1_num : (terrain1_num + 1);
                    break;
                case 2:
                    if(imageInfo.overlapRate > 0.6){
                        terrain2_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        terrain2_heightSuccessNum++;
                    }
                    terrain2_num = (imageInfo.evaHeight == -1) ? terrain2_num : (terrain2_num + 1);
                    break;
                case 3:
                    if(imageInfo.overlapRate > 0.6){
                        terrain3_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        terrain3_heightSuccessNum++;
                    }
                    terrain3_num = (imageInfo.evaHeight == -1) ? terrain3_num : (terrain3_num + 1);
                    break;
                case 5:
                    if(imageInfo.overlapRate > 0.6){
                        terrain5_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        terrain5_heightSuccessNum++;
                    }
                    terrain5_num = (imageInfo.evaHeight == -1) ? terrain5_num : (terrain5_num + 1);
                    break;
                case 7:
                    if(imageInfo.overlapRate > 0.6){
                        terrain7_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        terrain7_heightSuccessNum++;
                    }
                    terrain7_num = (imageInfo.evaHeight == -1) ? terrain7_num : (terrain7_num + 1);
                    break;
                default:
                    SDKLog::logE("Terrain Exception! Terrain = ", imageInfo.terrain);
                    break;
            }
    
            //统计干扰类型结果
            switch(imageInfo.jam){
                case 1:
                    if(imageInfo.overlapRate > 0.6){
                        jam1_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        jam1_heightSuccessNum++;
                    }
                    jam1_num = (imageInfo.evaHeight == -1) ? jam1_num : (jam1_num + 1);
                    break;
                case 2:
                    if(imageInfo.overlapRate > 0.6){
                        jam2_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        jam2_heightSuccessNum++;
                    }
                    jam2_num = (imageInfo.evaHeight == -1) ? jam2_num : (jam2_num + 1);
                    break;
                case 3:
                    if(imageInfo.overlapRate > 0.6){
                        jam3_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        jam3_heightSuccessNum++;
                    }
                    jam3_num = (imageInfo.evaHeight == -1) ? jam3_num : (jam3_num + 1);
                    break;
                case 4:
                    if(imageInfo.overlapRate > 0.6){
                        jam4_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        jam4_heightSuccessNum++;
                    }
                    jam4_num = (imageInfo.evaHeight == -1) ? jam4_num : (jam4_num + 1);
                    break;
                default:
                    SDKLog::logE("Jam Exception! Jam = ", imageInfo.jam);
                    break;
            }
    
            //统计平台结果
            switch(imageInfo.platform){
                case 1:
                    if(imageInfo.overlapRate > 0.6){
                        platform1_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        platform1_heightSuccessNum++;
                    }
                    platform1_num = (imageInfo.evaHeight == -1) ? platform1_num : (platform1_num + 1);
                    break;
                case 2:
                    if(imageInfo.overlapRate > 0.6){
                        platform2_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        platform2_heightSuccessNum++;
                    }
                    platform2_num = (imageInfo.evaHeight == -1) ? platform2_num : (platform2_num + 1);
                    break;
                case 3:
                    if(imageInfo.overlapRate > 0.6){
                        platform3_echoSuccessNum++;
                    }
                    if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
                        platform3_heightSuccessNum++;
                    }
                    platform3_num = (imageInfo.evaHeight == -1) ? platform3_num : (platform3_num + 1);
                    break;
                default:
                    SDKLog::logE("Platform Exception! Platform = ", imageInfo.platform);
                    break;
            }
        }
        

        txtOutput << infoStr << endl;
        SDKLog::log("imageINfo toString = ", imageInfo.toString());

        //统计不同测试用例数据，每250帧为一条数据
        SDKLog::log("imageInfo.num % 250 = ", numTemp % 250);
        if(numTemp !=0 && numTemp % 250 == 0){
            caseStr.append("###EchoSuccessNum = " + QString::number(hundredAccNum) + "\n");
            caseStr.append("###EchoLostNum = " + QString::number(hundredLostNum) + "\n");
            caseStr.append("###Echo Rate = " + QString::number((double)hundredAccNum / (250 - hundredLostNum)));
            txtOutput << caseStr << endl;
            SDKLog::logE("caseStr = ", caseStr);
            numTemp = 0;
            hundredAccNum = 0;
            hundredLostNum = 0;
        }
        
        /** syc 打印写文件线程id
        * QString LogInfo;
        * LogInfo.sprintf("%p", QThread::currentThread());
        * SDKLog::logE("WriteThread: cur thread id = ", LogInfo);
        */

        return;
    }
}

/**
 * @brief WriteResultFileThread::updateParam 更新写入参数
 * @param imageInfo 图片参数
 */
void WriteResultFileThread::updateParam(QString testCaseNum, PicInfo imageInfo){
    mTestCaseNum = testCaseNum;
    mImageInfo = imageInfo;
    writeResult2File(mImageInfo);
}

//TO-DO 写入RMSE等统计结果

/**
 * @brief WriteResultFileThread::closeResultFile 一次测试结束，关闭文件
 */
void WriteResultFileThread::closeResultFile(QString resultStatisticStr){

    QTextStream txtOutput(mResultFile);

    resultStatisticStr.append("\n################################################################\n");
    //信噪比统计
    resultStatisticStr.append("snr10_num = " + QString::number(snr10_num) 
                        + " snr10_echoSuccessNum = " + QString::number(snr10_echoSuccessNum)
                        + " snr10_heightSuccessNum = " + QString::number(snr10_heightSuccessNum)
                        + "\n");
    resultStatisticStr.append("snr15_num = " + QString::number(snr15_num)
                        + " snr15_echoSuccessNum = " + QString::number(snr15_echoSuccessNum)
                        + " snr15_heightSuccessNum = " + QString::number(snr15_heightSuccessNum)
                        + "\n");
    resultStatisticStr.append("snr20_num = " + QString::number(snr20_num)
                        + " snr20_echoSuccessNum = " + QString::number(snr20_echoSuccessNum)
                        + " snr20_heightSuccessNum = " + QString::number(snr20_heightSuccessNum)
                        + "\n");
        //正确率统计
    resultStatisticStr.append("snr10 echoSuccessRate = " + QString::number((double)snr10_echoSuccessNum / snr10_num)
                        + " snr10 heightSuccessRate = " + QString::number((double)snr10_heightSuccessNum / snr10_num));
    resultStatisticStr.append("\nsnr15 echoSuccessRate = " + QString::number((double)snr15_echoSuccessNum / snr15_num)
                        + " snr15 heightSuccessRate = " + QString::number((double)snr15_heightSuccessNum / snr15_num));
    resultStatisticStr.append("\nsnr20 echoSuccessRate = " + QString::number((double)snr20_echoSuccessNum / snr20_num)
                        + " snr20 heightSuccessRate = " + QString::number((double)snr20_heightSuccessNum / snr20_num));
    resultStatisticStr.append("\n");
    //地形统计
    resultStatisticStr.append("terrain1_num = " + QString::number(terrain1_num)
                        + " terrain1_echoSuccessNum = " + QString::number(terrain1_echoSuccessNum)
                        + " terrain1_heightSuccessNum = " + QString::number(terrain1_heightSuccessNum)
                        + "\n");
    resultStatisticStr.append("terrain2_num = " + QString::number(terrain2_num)
                        + " terrain2_echoSuccessNum = " + QString::number(terrain2_echoSuccessNum)
                        + " terrain2_heightSuccessNum = " + QString::number(terrain2_heightSuccessNum)
                        + "\n");
    resultStatisticStr.append("terrain3_num = " + QString::number(terrain3_num)
                        + " terrain3_echoSuccessNum = " + QString::number(terrain3_echoSuccessNum)
                        + " terrain3_heightSuccessNum = " + QString::number(terrain3_heightSuccessNum)
                        + "\n");
    resultStatisticStr.append("terrain5_num = " + QString::number(terrain5_num)
                        + " terrain5_echoSuccessNum = " + QString::number(terrain5_echoSuccessNum)
                        + " terrain5_heightSuccessNum = " + QString::number(terrain5_heightSuccessNum)
                        + "\n");
    resultStatisticStr.append("terrain7_num = " + QString::number(terrain7_num)
                        + " terrain7_echoSuccessNum = " + QString::number(terrain7_echoSuccessNum)
                        + " terrain7_heightSuccessNum = " + QString::number(terrain7_heightSuccessNum)
                        + "\n");
        //正确率统计
    resultStatisticStr.append("terrain1 echoSuccessRate = " + QString::number((double)terrain1_echoSuccessNum / terrain1_num)
                        + " terrain1 heightSuccessRate = " + QString::number((double)terrain1_heightSuccessNum / terrain1_num));
    resultStatisticStr.append("\nterrain2 echoSuccessRate = " + QString::number((double)terrain2_echoSuccessNum / terrain2_num)
                        + " terrain2 heightSuccessRate = " + QString::number((double)terrain2_heightSuccessNum / terrain2_num));
    resultStatisticStr.append("\nterrain3 echoSuccessRate = " + QString::number((double)terrain3_echoSuccessNum / terrain3_num)
                        + " terrain3 heightSuccessRate = " + QString::number((double)terrain3_heightSuccessNum / terrain3_num));
    resultStatisticStr.append("\nterrain5 echoSuccessRate = " + QString::number((double)terrain5_echoSuccessNum / terrain5_num)
                        + " terrain5 heightSuccessRate = " + QString::number((double)terrain5_heightSuccessNum / terrain5_num));
    resultStatisticStr.append("\nterrain7 echoSuccessRate = " + QString::number((double)terrain7_echoSuccessNum / terrain7_num)
                        + " terrain7 heightSuccessRate = " + QString::number((double)terrain7_heightSuccessNum / terrain7_num));
    resultStatisticStr.append("\n");
    //干扰类型统计
    resultStatisticStr.append("jam1_num = " + QString::number(jam1_num)
                        + " jam1_echoSuccessNum = " + QString::number(jam1_echoSuccessNum)
                        + " jam1_heightSuccessNum = " + QString::number(jam1_heightSuccessNum)
                        + "\n");
    resultStatisticStr.append("jam2_num = " + QString::number(jam2_num)
                        + " jam2_echoSuccessNum = " + QString::number(jam2_echoSuccessNum)
                        + " jam2_heightSuccessNum = " + QString::number(jam2_heightSuccessNum)
                        + "\n");
    resultStatisticStr.append("jam3_num = " + QString::number(jam3_num)
                        + " jam3_echoSuccessNum = " + QString::number(jam3_echoSuccessNum)
                        + " jam3_heightSuccessNum = " + QString::number(jam3_heightSuccessNum)
                        + "\n");
    resultStatisticStr.append("jam4_num = " + QString::number(jam4_num)
                        + " jam4_echoSuccessNum = " + QString::number(jam4_echoSuccessNum)
                        + " jam4_heightSuccessNum = " + QString::number(jam4_heightSuccessNum)
                        + "\n");
        //正确率统计
    resultStatisticStr.append("jam1 echoSuccessRate = " + QString::number((double)jam1_echoSuccessNum / jam1_num)
                        + " jam1 heightSuccessRate = " + QString::number((double)jam1_heightSuccessNum / jam1_num));
    resultStatisticStr.append("\njam2 echoSuccessRate = " + QString::number((double)jam2_echoSuccessNum / jam2_num)
                        + " jam2 heightSuccessRate = " + QString::number((double)jam2_heightSuccessNum / jam2_num));
    resultStatisticStr.append("\njam3 echoSuccessRate = " + QString::number((double)jam3_echoSuccessNum / jam3_num)
                        + " jam3 heightSuccessRate = " + QString::number((double)jam3_heightSuccessNum / jam3_num));
    resultStatisticStr.append("\njam4 echoSuccessRate = " + QString::number((double)jam4_echoSuccessNum / jam4_num)
                        + " jam4 heightSuccessRate = " + QString::number((double)jam4_heightSuccessNum / jam4_num));
    resultStatisticStr.append("\n");
    //平台统计
    resultStatisticStr.append("platform1_num = " + QString::number(platform1_num)
                        + " platform1_echoSuccessNum = " + QString::number(platform1_echoSuccessNum)
                        + " platform1_heightSuccessNum = " + QString::number(platform1_heightSuccessNum)
                        + "\n");
    resultStatisticStr.append("platform2_num = " + QString::number(platform2_num)
                        + " platform2_echoSuccessNum = " + QString::number(platform2_echoSuccessNum)
                        + " platform2_heightSuccessNum = " + QString::number(platform2_heightSuccessNum)
                        + "\n");
    resultStatisticStr.append("platform3_num = " + QString::number(platform3_num)
                        + " platform3_echoSuccessNum = " + QString::number(platform3_echoSuccessNum)
                        + " platform3_heightSuccessNum = " + QString::number(platform3_heightSuccessNum)
                        + "\n");
        //正确率统计
    resultStatisticStr.append("platform1 echoSuccessRate = " + QString::number((double)platform1_echoSuccessNum / platform1_num)
                        + " platform1 heightSuccessRate = " + QString::number((double)platform1_heightSuccessNum / platform1_num));
    resultStatisticStr.append("\nplatform2 echoSuccessRate = " + QString::number((double)platform2_echoSuccessNum / platform2_num)
                        + " platform2 heightSuccessRate = " + QString::number((double)platform2_heightSuccessNum / platform2_num));
    resultStatisticStr.append("\nplatform3 echoSuccessRate = " + QString::number((double)platform3_echoSuccessNum / platform3_num)
                        + " platform3 heightSuccessRate = " + QString::number((double)platform3_heightSuccessNum / platform3_num));
    resultStatisticStr.append("\n################################################################\n");

    //与软件结果平均差值统计
    resultStatisticStr.append("softHeight diff average sum = " + QString::number(softHeightTemp) + "\n");
    resultStatisticStr.append("softHeight diff average = " + QString::number((double) softHeightTemp / (snr10_num+snr15_num+snr20_num)) );

    txtOutput << resultStatisticStr << endl;
    //SDKLog::logE("closeResultFile, STR = \n", resultStatisticStr);

    mResultFile->flush();
    mResultFile->close();
}