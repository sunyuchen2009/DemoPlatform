#ifndef CUSTOMDYNAMICWIDGET_H
#define CUSTOMDYNAMICWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QTimer>
#include <QPainter>
#include <QFont>
#include "SDKLog.h"
#include "defines.h"
#include "dataUtils.h"

class QTimer;
class FrameWidget : public QWidget
{
    Q_OBJECT
public:

    explicit FrameWidget(QWidget *parent = 0);
    /*
    * 设置动画图标
    * 函数名：setAnimation
    * 参数 _pix：图标实例
    * 参数 _count：图标实例动画帧数
    * 参数 _msec：动画切帧速度 (毫秒级)
    */
    void setAnimation(const int _msec = 100);

    void init(const int frameRate);

    void startClockwise(void);
    void stop(void);
    void setLoop(const bool _isLoop = false) { isLoop_ = _isLoop; }

    //显示灰度图
    QPixmap getGrayscaleImg(PicInfo imageInfo, int width, int height, QImage::Format format);
    //根据图片序号更新picMap中的单张图片
    void updatePix(PicInfo mImage);
    //设置Widget状态
    void setStatus(int status);
    //图像增强开关
    void setImgEnhance(int status);

signals:
    void clockwiseFinished(void);

private slots:
    void updateClockwise(void);

protected:
    void paintEvent(QPaintEvent *);

private:
    bool isTimeOutflag;

    bool isLoop_;
    /*  图标列表数量  */
    short count_;
    /*  当前展示的图标下标  */
    short currentIndex;
    /*  控制顺时针槽定时器  */
    QTimer *timer;
    /*  当前展示的图标  */
    QPixmap currentPix;
    /*  图标列表  */

    int statusNow = -1;
    int isEnhanceImg = 0;
    QList<QPixmap> pixList;
    dataUtils du;
};

#endif // CUSTOMDYNAMICWIDGET_H
