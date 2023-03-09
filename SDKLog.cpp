#include "SDKLog.h"

using namespace std;

unsigned int SDKLog::isDebug = IS_DEBUG;

void SDKLog::log(QString msg, QString value){
    if(IS_INFO){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK:"
             << timestamp.toStdString()
             << msg.toStdString()
             << " -> "
             << value.toStdString()
             << endl;
    }
}

void SDKLog::log(string msg, string value){
    if(IS_INFO){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg
             << " -> "
             << value
             << endl;
    }
}

void SDKLog::log(string msg, int value){
    if(IS_INFO){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg
             << " -> "
             << value
             << endl;
    }
}

void SDKLog::logDouble(string msg, double value){
    if(IS_INFO){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg
             << " -> "
             << value
             << endl;
    }
}

void SDKLog::logLong(string msg, long value){
    if(IS_INFO){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg
             << " -> "
             << value
             << endl;
    }
}

void SDKLog::log(string msg, QByteArray value){
    if(IS_INFO){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg;
        QString str;
        for(int i=0; i<value.size(); i++){
            str += QString("%1").arg((unsigned char)value.at(i), 2, 16, QLatin1Char('0') ) + ' ';
        }
        cout << str.toStdString()
             << endl;
    }
}

void SDKLog::log(string msg){
    if(IS_INFO){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg
             << endl;
    }
}

/********************************* LogE *********************************/

void SDKLog::logE(string msg){
    if(IS_RELEASE){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg
             << endl;
    }
}

void SDKLog::logE(string msg, int value){
    if(IS_RELEASE){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg
             << " -> "
             << value
             << endl;
    }
}

void SDKLog::logE(QString msg, QString value){
    if(IS_RELEASE){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK:"
             << timestamp.toStdString()
             << msg.toStdString()
             << " -> "
             << value.toStdString()
             << endl;
    }
}

void SDKLog::logE(string msg, string value){
    if(IS_RELEASE){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg
             << " -> "
             << value
             << endl;
    }
}

void SDKLog::logDoubleE(string msg, double value){
    if(IS_RELEASE){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg
             << " -> "
             << value
             << endl;
    }
}

/********************************* LogD *********************************/

void SDKLog::logD(string msg){
    if(IS_DEBUG){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg
             << endl;
    }
}

void SDKLog::logD(string msg, int value){
    if(IS_DEBUG){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg
             << " -> "
             << value
             << endl;
    }
}

void SDKLog::logD(QString msg, QString value){
    if(IS_DEBUG){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK:"
             << timestamp.toStdString()
             << msg.toStdString()
             << " -> "
             << value.toStdString()
             << endl;
    }
}

void SDKLog::logD(string msg, string value){
    if(IS_DEBUG){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg
             << " -> "
             << value
             << endl;
    }
}

void SDKLog::logDoubleD(string msg, double value){
    if(IS_DEBUG){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg
             << " -> "
             << value
             << endl;
    }
}

void SDKLog::logD(string msg, QByteArray value){
    if(IS_DEBUG){
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString timestamp = current_date_time.toString("hh:mm:ss.zzz") + " ";
        cout << "socketSDK: "
             << timestamp.toStdString()
             << msg;
        QString str;
        for(int i=0; i<value.size(); i++){
            str += QString("%1").arg((unsigned char)value.at(i), 2, 16, QLatin1Char('0') ) + ' ';
        }
        cout << str.toStdString()
             << endl;
    }
}