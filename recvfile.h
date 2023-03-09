#ifndef RECVFILE_H
#define RECVFILE_H

#include <QThread>
#include <QTcpSocket>
#include "defines.h"
#include "dataUtils.h"

class recvFile : public QThread
{
    Q_OBJECT
public:
    explicit recvFile(int fileType, int fileNum, QByteArray fileData, QObject *parent = nullptr);
protected:
    void run() override;
signals:
    void over();
    void curprogress(int);
private:
    const QString PIC_SUFFIX = ".bmp";
    const QString TXT_SUFFIX = ".txt";
    //const QString ABS_ADDR = "/forlinx/qtbin/socket/";
    const QString ABS_ADDR = "";
    QByteArray mData;
    QString mType;
    int mFileNum;
    dataUtils du;
};

#endif // RECVFILE_H
