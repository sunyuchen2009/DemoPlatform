#include "recvfile.h"

#include "SDKLog.h"
#include "dataUtils.h"
#include <QFile>
#include <iostream>
#include <QDateTime>
//#include <QRandomGenerator>

/**
 * @brief recvFile::recvFile
 * @param fileType 接收文件类型
 * @param fileData 接收文件数据
 * @param parent
 */
recvFile::recvFile(int fileType, int fileNum, QByteArray fileData, QObject *parent)
    : QThread{parent}
{
    mData = fileData;
    mFileNum = fileNum;

    switch (fileType) { //判断接收文件类型
        case RECV_TXT:
            mType = TXT_SUFFIX;
            du.normalization(fileData);
            break;
        case RECV_PICS:
            mType = PIC_SUFFIX;
            break;
        default:
            mType = nullptr;
            break;
    }

    SDKLog::log("recvFile<<<<<<<<<<<<<<<");
}

void recvFile::run(){
    QString path;
    QByteArray mFileUnit;
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString timestamp = "-" + current_date_time.toString("hh_mm_ss.zzz");
    int currentPicSize = dataUtils::picMap.size();

    long mFileUnitLen = (mFileNum == 0) ? 0 : mData.size() / mFileNum;

    if(!mType.compare(PIC_SUFFIX)){
        for(int i=currentPicSize + 1; i<=currentPicSize + mFileNum; i++){
            path = ABS_ADDR + "file" + QString::number(i) + timestamp + mType;
            mFileUnit = mData.left(mFileUnitLen);
            mData = mData.mid(mFileUnitLen);
            //循环写入文件
            QFile *pic = new QFile(path);

            //du.savePic2Map(path);
            pic->open(QFile::WriteOnly);

            SDKLog::log("open pic file", i);

            pic->write(mFileUnit);
            pic->close();
            pic->deleteLater();
        }
    }else if(!mType.compare(TXT_SUFFIX)){
        path = ABS_ADDR + "pic_txt" + timestamp + mType;
        QFile *pic_txt = new QFile(path);
        pic_txt->open(QFile::WriteOnly);

        SDKLog::log("open txt file", path);

        pic_txt->write(mData.left(mFileUnitLen));
        mData = mData.mid(mFileUnitLen);    //裁剪掉已经写入文件的mData
        pic_txt->close();
        pic_txt->deleteLater();
    }else{
        SDKLog::log("mType error! mType = ", mType);
    }


    emit over();
    //进入事件循环
    exec();
}
