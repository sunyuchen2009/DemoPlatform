#include <QMetaType>

typedef struct {
//    QString evaluateHeight; //估计高度
//    QString realHeight; //真实高度
//    QString confidenceCoe; //置信度
//    QString rmse; //均方根误差
    QString leftLabel; //左边显示类型
    QString rightData; //右边显示数据
} itemData;

Q_DECLARE_METATYPE(itemData)
