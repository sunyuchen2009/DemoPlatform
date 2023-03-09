#ifndef DATAUTILS_H
#define DATAUTILS_H
#include <QHash>
#include <QString>
#include <iomanip>
#include <QImage>
#include <QPainter>
#include <queue>
#include "SDKLog.h"
#include "defines.h"
#include <math.h>
class dataUtils{
public:
    //RMSE中间结果缓存
    unsigned long RMSETempSum = 0;

    //预测正确样本个数
    static unsigned int successNum;

    //回波辨识成功个数
    static unsigned int echoSuccessNum;

    //回波丢失样本个数
    static unsigned int echoLostNum;

    //>200m样本个数
    static unsigned int twoSuccessNum;

    //图像增强LUT
    static int lutArray[256];

    /* 图片处理 */
    QByteArray convert16ImgTo8Img(QByteArray sixteenImg, int isEnhanceImg);   //16bit灰度图转8bit灰度图
    QImage editImage(QImage originImage, int eHeight, int rHeight); //QImage编辑函数
    QByteArray histgram(QByteArray imageData);

    /* FPGA结果处理 */
    QStringList getResult(long mResult);                  //获取计算结果
    QString getConfiStrFromShort(short confi);            //获取置信度字符串
    double getLostRate(int size);                         //获取回波丢失率结果
    double getRMSE(int size);                             //获取RMSE结果
    void putTempResult(PicInfo imageInfo);                //缓存中间结果
    double getHeightRecogRate(int size);                  //获取高度辨识率结果
    double getEchoRecogRate(int size);                      //获取回波辨识率
    double getOverlapRate(int evaHeight, int realHeight);     //获取回波重叠率
    void clearTempCache();

    /* 字节操作方法 */
    long makeLongResult(short result0, short result1);
    short makeChar2Short(char byte0, char byte1);

};

#endif // DATAUTILS_H
