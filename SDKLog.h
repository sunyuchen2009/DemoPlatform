#ifndef SDKLOG_H
#define SDKLOG_H

#define IS_INFO     0
#define IS_RELEASE  1
#define IS_DEBUG    0


#include <QString>
#include <string>
#include <iostream>
#include <QDateTime>

using namespace std;
class SDKLog{
public:
    static unsigned int isDebug;

    /* info mode 所有log全部打开 */
    static void log(QString msg, QString value);
    static void log(string msg, string value);
    static void log(string msg);
    static void log(string msg, int value);
    static void logLong(string msg, long value);
    static void log(string msg, QByteArray value);
    static void logDouble(string msg, double value);

    /* release mode 仅显示错误log */
    static void logE(string msg);
    static void logE(string msg, string value);
    static void logE(QString msg, QString value);
    static void logE(string msg, int value);
    static void logDoubleE(string msg, double value);

    /* debug mode 显示部分调试信息 */
    static void logD(string msg);
    static void logD(string msg, string value);
    static void logD(QString msg, QString value);
    static void logD(string msg, int value);
    static void logD(string msg, QByteArray value);
    static void logDoubleD(string msg, double value);


};

#endif // SDKLOG_H
