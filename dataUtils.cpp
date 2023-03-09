/************************************************************************************************
*--------------File Info-------------------------------------------------------------------------
* File name:            dataUtils.cpp
* Last modified Date:   2022-09-07
* Last Version:
* Descriptions:         数据处理类
*                           1.重叠率计算
*                           2.RMSE统计结果计算
*                           3.置信度计算
*                           4.回波动态追踪的图片绘制
*--------------------------------------------------------------------------------------------------
* Created by:           syc
* Created date:         2022-09-07
* Version:              1.0.0
* Descriptions:         The original version
*
*--------------------------------------------------------------------------------------------------
* Modified by:
* Modified date:
* Version:
* Descriptions:
*--------------------------------------------------------------------------------------------------
***************************************************************************************************/

#include "dataUtils.h"

//定义静态变量，分配内存
unsigned int dataUtils::echoLostNum = 0;
unsigned int dataUtils::successNum = 0;
unsigned int dataUtils::echoSuccessNum = 0;
unsigned int dataUtils::twoSuccessNum = 0;
int dataUtils::lutArray[] = {0,15,22,27,31,35,39,42,45,47,50,52,55,57,59,61,63,65,67,69,71,73,74,76,78,79,81,82,84,85,87,88,90,91,93,94,95,97,98,99,100,102,103,104,105,107,108,109,110,111,112,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,141,142,143,144,145,146,147,148,148,149,150,151,152,153,153,154,155,156,157,158,158,159,160,161,162,162,163,164,165,165,166,167,168,168,169,170,171,171,172,173,174,174,175,176,177,177,178,179,179,180,181,182,182,183,184,184,185,186,186,187,188,188,189,190,190,191,192,192,193,194,194,195,196,196,197,198,198,199,200,200,201,201,202,203,203,204,205,205,206,206,207,208,208,209,210,210,211,211,212,213,213,214,214,215,216,216,217,217,218,218,219,220,220,221,221,222,222,223,224,224,225,225,226,226,227,228,228,229,229,230,230,231,231,232,233,233,234,234,235,235,236,236,237,237,238,238,239,240,240,241,241,242,242,243,243,244,244,245,245,246,246,247,247,248,248,249,249,250,250,251,251,252,252,253,253,254,255};


/**
 * @brief dataUtils::makeLongResult make long result to map
 * @param result0  the first result
 * @param result1  the second result
 * @return a whole long result
 */ 
long dataUtils::makeLongResult(short result0, short result1){
    long mResult = 0;
    mResult = mResult | result0;
    mResult = mResult << 16;
    mResult = mResult | result1;
    return mResult;
}

/**
* @brief dataUtils::makeChar2Short 将两个char合并为一个short，小端模式
* @param byte0  第一个char
* @param byte1  第二个char
* @return 合并结果
*/
short dataUtils::makeChar2Short(char byte0, char byte1){
    short mResult = 0;
    mResult = mResult | byte0;
    mResult = mResult << 8;
    mResult = mResult | byte1;
    return mResult;
}

/**
* @brief dataUtils::convert16ImgTo8Img 将16bit的灰度图转为8bit的灰度图
* @param sixteenImg 16bit的灰度图数据
* @return 8bit的灰度图数据
*/
QByteArray dataUtils::convert16ImgTo8Img(QByteArray sixteenImg, int isEnhanceImg){
    int i, j = 0;
    QByteArray eightImg;
    QString outHex;
    eightImg.resize(sixteenImg.size() / 2);

    //直接取高8位
    while(i < sixteenImg.size()){
        eightImg[j] = sixteenImg.at(i + 1);
        if(isEnhanceImg){
            outHex = QString("%1").arg(lutArray[eightImg[j]], 2, 16, QLatin1Char('0'));
            eightImg[j] = outHex.toUInt(nullptr, 16);
        }
        i += 2;
        j++;
    }

    return eightImg;
}

/**
 * @brief dataUtils::histgram 直方图均衡化
 * @param imageData 8位的灰度图数据
 */
QByteArray dataUtils::histgram(QByteArray imageData){
    QByteArray outputArray;
    outputArray.resize(imageData.size());

    // int c = 255;
    // int b = 100;
    // int a = 30;
    
    // int f3 = 255;
    // int f2 = 200;
    // int f1 = 60;
    // //图像增强LUT数组
    // int lutArray[256];
    // for(int value = 0; value < 256; value ++){
    //     if(value < 10){
    //         lutArray[value] = f1 * value / a;
    //     }else if(value >= 10 && value < 75){
    //         lutArray[value] = (f2-f1) * (value - a) / (b-a) + f1;
    //     }else if(value >= 75 && value < 255){
    //         lutArray[value] = (f3-f2) * (value - b) / (c-b) + f2;
    //     }
    // }

    //直方图均衡化,更新原图每个点的像素值
    for(int i=0; i<imageData.size(); i++){
        int value = imageData.at(i);
        QString outHex;
    
        //outHex = QString("%1").arg((int)(sqrt(value/255.0)*255.0), 2, 16, QLatin1Char('0'));
        outHex = QString("%1").arg(lutArray[value], 2, 16, QLatin1Char('0'));
        // outHex = QString("%1").arg(lutArray[value], 2, 16, QLatin1Char('0'));
        
        //QString outHex = (value >= 30) ? QString("%1").arg(value + 30, 2, 16, QLatin1Char('0')) : QString("%1").arg(value, 2, 16, QLatin1Char('0'));

        outputArray[i] = outHex.toUInt(nullptr, 16);
    }

    return outputArray;
}

/*
* @brief dataUtils::getResult 将resMap中的long结果分离成两个结果
* @param mResult 32位的结果，高16位是估计高度值，低16位是置信度
* @return 返回QStringList，位置0是高度，位置1是置信度
*/
QStringList dataUtils::getResult(long mResult){
    QStringList resultList;
    short zeroTemp = 0;
    //获取高16位的预测高度值
    short heightResult = zeroTemp | mResult >> 16;
    resultList.insert(0, QString::number(heightResult));
    //获取低16位的置信度
    double confiResult = (double) (zeroTemp | mResult << 16 >> 16) / 32768;
    resultList.insert(1, QString::number(confiResult * 100.0, 'g', 4) + " %" );
    
    SDKLog::log("get resMap result, heightResult = ", resultList.at(0));
    SDKLog::log("get resMap result, origin confiResult = ", (zeroTemp | mResult << 16 >> 16));
    SDKLog::log("get resMap result, confiResult = ", resultList.at(1));

    return resultList;
}

/*
* @brief dataUtils::getConfiStrFromShort 根据FPGA返回的short类型结果，得到置信度的字符串
* @param confi FPGA返回的置信度结果
* @return 百分比的字符串，保留小数点后两位小数
*/
QString dataUtils::getConfiStrFromShort(short confi){
    double confiResult = (double) confi / 32768;
    QString confiStr = QString::number(confiResult * 100.0, 'g', 4) + " %";
    return confiStr;
}

/**
 * @brief dataUtils::editImage 编辑QImage，按照估计高度/10得到列坐标，左右扩展25列框出回波
 * @param originImage
 * @param eHeight
 * @return 处理好的图片
 */
QImage dataUtils::editImage(QImage originImage, int eHeight, int rHeight){
    //默认仅绘制真实回波框
    int isShowEva = 0;
    int isShowReal = 1;
    if(eHeight < 0 || eHeight > PIC_WIDTH*10){
        isShowEva = 0;
        SDKLog::log("EditImage() failed! eHeight is illegal! eHeight = ", eHeight);
    }else if(rHeight < 0 || rHeight > PIC_WIDTH*10){
        isShowReal = 0;
        SDKLog::log("EditImage() failed! rHeight is illegal! rHeight = ", rHeight);
    }else{
        isShowReal = 1;
        isShowEva = 1;
    }  
    
    //真实高度非法，直接返回
    if(!isShowReal){
        return originImage;
    }

    QImage newImage = originImage.convertToFormat(QImage::Format_RGB555);
    eHeight = eHeight / 10;
    rHeight = rHeight / 10;
    QPainter painter(&newImage);

    //绘制预测回波框
    if(isShowEva){
        painter.setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(QPoint(eHeight - 25, 2), QPoint(eHeight + 25, 2));
        painter.drawLine(QPoint(eHeight - 25, PIC_HEIGHT-2), QPoint(eHeight + 25, PIC_HEIGHT-2));
        painter.drawLine(QPoint(eHeight - 25, 2), QPoint(eHeight - 25, PIC_HEIGHT-2));
        painter.drawLine(QPoint(eHeight + 25, 2), QPoint(eHeight + 25, PIC_HEIGHT-2));
    }

    //绘制真实回波框
    if(isShowReal){
        painter.setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(QPoint(rHeight - 25, 2), QPoint(rHeight + 25, 2));
        painter.drawLine(QPoint(rHeight - 25, PIC_HEIGHT-2), QPoint(rHeight + 25, PIC_HEIGHT-2));
        painter.drawLine(QPoint(rHeight - 25, 2), QPoint(rHeight - 25, PIC_HEIGHT-2));
        painter.drawLine(QPoint(rHeight + 25, 2), QPoint(rHeight + 25, PIC_HEIGHT-2));
        painter.end();
    }
    
    /** syc 操作像素画框
    * for(int i=eHeight-25; i<eHeight+25; i++){
    *     newImage.setPixel(i, 1, qRgb(255,0,0));
    *     newImage.setPixel(i, PIC_HEIGHT-1, qRgb(255,0,0));
    * }
    * for(int j=0; j<PIC_HEIGHT; j++){
    *     newImage.setPixel(eHeight - 25, j, qRgb(255,0,0));
    *     newImage.setPixel(eHeight + 25, j, qRgb(255,0,0));
    * }
    */
    return newImage;
    
}

/**
* @brief dataUtils::clearPicMap 清除所有缓存
*/
void dataUtils::clearTempCache(){
    RMSETempSum = 0;
    successNum = 0;
    echoSuccessNum = 0;
    echoLostNum = 0;
    twoSuccessNum = 0;
    return;
}

/**
 * @brief dataUtils::getOverlapRate 获取回波辨识率，当预测框框和真实框框重叠面积 > 0.6时判定为识别成功
 * @param evaHeight  预测高度
 * @param realHeight 真实高度
 */
double dataUtils::getOverlapRate(int evaHeight, int realHeight){
    int big = 0;
    int small = 0;
    double overlapRate = 0;

    //转化高度为坐标
    evaHeight = evaHeight / 10;
    realHeight = realHeight / 10;

    //回波重叠率为0%的情况
    if(abs(evaHeight - realHeight) >= 50){
        return overlapRate;
    }

    if(evaHeight > realHeight){
        big = evaHeight;
        small = realHeight;
    }else{
        big = realHeight;
        small = evaHeight;
    }
    overlapRate = (double) (small - big + 50) / 50;
    SDKLog::log("getOverlapRate small = ", small);
    SDKLog::log("getOverlapRate big = ", big);
    SDKLog::logDouble("getOverlapRate", overlapRate);
    return overlapRate;
}

/**
 * @brief dataUtils::putTempResult 缓存中间结果
 * @param imageInfo 图片信息
 */
void dataUtils::putTempResult(PicInfo imageInfo){
    if(imageInfo.evaHeight == -1){ //回波丢失
        echoLostNum++;
    }else{
        //缓存RMSE中间结果
        RMSETempSum += (imageInfo.realHeight - imageInfo.evaHeight) * (imageInfo.realHeight - imageInfo.evaHeight);

        //缓存<30m指标
        if(abs(imageInfo.evaHeight - imageInfo.realHeight) < 30){
            successNum++;
        }
        //缓存回波辨识指标
        if(imageInfo.overlapRate >= 0.6){
            echoSuccessNum++;
        }
        //缓存<200m指标
        if(abs(imageInfo.evaHeight - imageInfo.realHeight) <= 200){
            twoSuccessNum++;
        }
    }
}

/**
 * @brief dataUtils::getEchoRecogRate 获取回波辨识率，当预测框框和真实框框重叠面积 > 0.6时判定为识别成功
 * @param size  本次统计的样本个数
 * @return 回波识别率结果
 */
double dataUtils::getEchoRecogRate(int size){
    //size = size - echoLostNum;
    double EchoRecognitionRate = echoSuccessNum * 1.0 / size;
    SDKLog::logDouble("Get EchoRecognitionRate = ", EchoRecognitionRate);
    return EchoRecognitionRate;
}

/**
 * @brief dataUtils::getHeightRecogRate 获取高度识别率结果，预测高度与真实高度小于30计为成功
 * @param size 本次统计的样本个数
 * @return 高度识别率结果
 */
double dataUtils::getHeightRecogRate(int size){
    //size = size - echoLostNum;
    double heightRecognitionRate = successNum * 1.0 / size;
    SDKLog::logDouble("Get heightRecognitionRate = ", heightRecognitionRate);
    return heightRecognitionRate;
}

/**
 * @brief dataUtils::getLostRate 获取回波丢失率统计结果
 * @param size 本次统计的样本个数
 * @return 回波丢失率结果
 */
double dataUtils::getLostRate(int size){
    double lostRate = (double)echoLostNum / size;
    return lostRate;
}

/**
 * @brief dataUtils::getRMSE 获取RMSE统计结果
 * @param size 本次统计的样本个数
 * @return RMSE结果
 */
double dataUtils::getRMSE(int size){
    size = size - echoLostNum;
    double rmseResult = sqrt(RMSETempSum * 1.0 / size);
    //SDKLog::logDoubleE("Get RMSE = ", rmseResult);
    return rmseResult;
}


