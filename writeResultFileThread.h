#ifndef WRITERESULTFILETHREAD_H
#define WRITERESULTFILETHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QFile>
#include <iostream>
#include <QString>
#include <QDateTime>
#include <QTextStream>

#include "SDKLog.h"
#include "dataUtils.h"
#include "defines.h"

class WriteResultFileThread : public QThread
{
    Q_OBJECT
public:
    explicit WriteResultFileThread(QObject *parent = nullptr);

protected:
    void run() override;

signals:
    void over();

public slots:
    void updateParam(QString testCaseNum, PicInfo imageInfo);
    void closeResultFile(QString resultStatisticStr);

private:
    QFile *mResultFile;
    PicInfo mImageInfo;
    dataUtils du;
    QString mTestCaseNum;

    //测试用例统计数
    int numTemp = 0;
    int hundredAccNum = 0;
    int hundredLostNum = 0;

    //统计信噪比结果参数
    int snr10_num = 0;
    int snr10_echoSuccessNum = 0;
    int snr10_heightSuccessNum = 0;
    int snr15_num = 0;
    int snr15_echoSuccessNum = 0;
    int snr15_heightSuccessNum = 0;
    int snr20_num = 0;
    int snr20_echoSuccessNum = 0;
    int snr20_heightSuccessNum = 0;

    //统计地形结果参数
    int terrain1_num = 0;
    int terrain1_echoSuccessNum = 0;
    int terrain1_heightSuccessNum = 0;
    int terrain2_num = 0;
    int terrain2_echoSuccessNum = 0;
    int terrain2_heightSuccessNum = 0;
    int terrain3_num = 0;
    int terrain3_echoSuccessNum = 0;
    int terrain3_heightSuccessNum = 0;
    int terrain5_num = 0;
    int terrain5_echoSuccessNum = 0;
    int terrain5_heightSuccessNum = 0;
    int terrain7_num = 0;
    int terrain7_echoSuccessNum = 0;
    int terrain7_heightSuccessNum = 0;

    //统计干扰类型结果参数
    int jam1_num = 0;
    int jam1_echoSuccessNum = 0;
    int jam1_heightSuccessNum = 0;
    int jam2_num = 0;
    int jam2_echoSuccessNum = 0;
    int jam2_heightSuccessNum = 0;
    int jam3_num = 0;
    int jam3_echoSuccessNum = 0;
    int jam3_heightSuccessNum = 0;
    int jam4_num = 0;
    int jam4_echoSuccessNum = 0;
    int jam4_heightSuccessNum = 0;

    //统计平台结果参数
    int platform1_num = 0;
    int platform1_echoSuccessNum = 0;
    int platform1_heightSuccessNum = 0;
    int platform2_num = 0;
    int platform2_echoSuccessNum = 0;
    int platform2_heightSuccessNum = 0;
    int platform3_num = 0;
    int platform3_echoSuccessNum = 0;
    int platform3_heightSuccessNum = 0;

    unsigned long softHeightTemp = 0;

    unsigned int resultFileNum = 0;
    const QString ROOT_PATH = "/forlinx/qtbin/socket/";
    void writeResult2File(PicInfo imageInfo);

    //写入Lost数据
    QString lostFolderPath = "/forlinx/qtbin/socket/lostImage/";
    void writeLost2File(PicInfo imageInfo);
};

#endif // WRITERESULTFILETHREAD_H
