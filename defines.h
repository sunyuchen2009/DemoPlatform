#ifndef DEFINES_H
#define DEFINES_H
#include <string>

#define GET_STATUS  0 //获取硬件状态
#define RECV_PICS   1 //接收图片
#define RECV_TXT    2 //接收txt
#define GET_RESULT  3 //响应NI端并返回预测结果
#define ERROR       4 //返回错误信息
#define ERRO_STATUS -1

/* 实施数据显示String */
#define STR_30M           "3σ<30m:"
#define STR_ECHO_SUCCESS  "回波辨识率:"
#define STR_ECHO_LOST     "回波丢失率:"
#define STR_RMSE          "RMSE:"

/* 图片宽高 */
#define PIC_WIDTH 600	//图片宽度
#define PIC_HEIGHT 64  //图片高度

/* frameWidget状态码 */
#define FRAME_NOT_START 0   //未开始工作
#define FRAME_ECHO_LOST 1   //回波丢失
#define FRAME_WORKING   2   //正常传输数据

/* 主窗口状态码 */
#define MAIN_IDLE 0
#define MAIN_BUSY 1

/* SOCKET状态码 */
#define SOCKET_NOT_CONNECTED 0
#define SOCKET_CONNECTED     1
#define SOCKET_TRANSPORTING  2
#define SOCKET_ABNORMAL      3

/* 主窗口状态字符串 */
#define SOCKET_NOT_CONNECTED_STR "未连接"
#define SOCKET_CONNECTED_STR     "已连接"
#define SOCKET_TRANSPORTING_STR  "传输中"
#define SOCKET_ABNORMAL_STR      "状态异常"

/* FPGA done 状态码 */
#define LAST_TIME       1
#define NOT_LAST_TIME   0

typedef struct {
    /* 包头信息 */
    unsigned int num;   	   //图片序号
    int realHeight;            //高度的真实值
    int snr;                   //信噪比
    int jam;                   //干扰类型
    int terrain;               //地形
    int platform;              //平台
    int softHeight = -1;       //软件高度结果

    /* FPGA结果 */
    int evaHeight = -1;         //图片预测高度
    QString confi = "-1";       //置信度
    double overlapRate = -1;    //回波重叠率

    /* 图片数据 */
    QByteArray image;          //图片数据

    std::string toString() {
        return "Num:" + std::to_string(num) 
            + " realHeight:" + std::to_string(realHeight)
            + " snr:" + std::to_string(snr)
            + " jam:" + std::to_string(jam)
            + " terrain:" + std::to_string(terrain)
            + " platform:" + std::to_string(platform)
            + " evaHeight:" + std::to_string(evaHeight)
            + " confi:" + confi.toStdString()
            + " overlapRate:" + std::to_string(overlapRate)
            + " softHeight:" + std::to_string(softHeight);
    }
} PicInfo;

#endif // DEFINES_H
