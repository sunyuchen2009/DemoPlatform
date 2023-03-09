#include "frameWidget.h"

FrameWidget::FrameWidget(QWidget *parent) : QWidget(parent), isLoop_(true), currentIndex(0) {
    statusNow = FRAME_NOT_START;
}

/**
* 调用update()后会执行此函数中的代码，完成图像的重新绘制
*/
void FrameWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    //设置字体
    QFont font("宋体", 15, QFont::Bold, false);
    painter.setFont(font);
    switch (statusNow) {
        case FRAME_NOT_START:
            painter.drawText(280, 100, "测试未开始...");
            break;
        case FRAME_ECHO_LOST:
            painter.drawText(280, 100, "回波丢失");
            break;
        case FRAME_WORKING:
            painter.drawPixmap(rect(), currentPix);
            break;
        default:
            SDKLog::log("Frame Widget abnomal! status = ", statusNow);
            break;
    }
}

/**
* 根据传入的序号来显示图片，若调用过快，需要等待timer标志触发再替换
* @param picMapNum: picMap中的图片序号
*/
void FrameWidget::updatePix(PicInfo mImage){
    //currentIndex = picMapNum;
    currentPix = getGrayscaleImg(mImage, PIC_WIDTH, PIC_HEIGHT, QImage::Format_Grayscale8);
    
    //SDKLog::logD("currentIndex = ", currentIndex);
    //du.clearOnePic();
    update();
}

/**
* 将16bit的灰度图转为Pixmap
* @param imageData: 图片的原始数据，不包括数据头
* @param width:  图片宽度
* @param height: 图片高度
* @param format: 图片的数据格式
*/
QPixmap FrameWidget::getGrayscaleImg(PicInfo imageInfo, int width, int height, QImage::Format format){
    /** 依次渲染单个像素（8bit）
    * for(int i=0; i<width; i++){
    *    for(int j=0; j<height; j++){
    *     uint pixelval = imageData[i + j*width];
    *     QRgb color = qRgb(pixelval, pixelval, pixelval);
    *     image.setPixel( i, (height - 1 - j), color);
    *    }
    * }
    */

    /** 使用16位灰度图格式，在Linux无法使用
    * SDKLog::log("getGrayscaleImg, imageData size = ", imageData.size());
    * QImage image = QImage((const unsigned char*)imageData.data(), width, height, format);
    * //image = image.mirrored(true, false); //水平镜像
    * QPixmap pix = QPixmap::fromImage(image);
    * SDKLog::log("getGrayscaleImg, pix isNull = ", pix.isNull());
    */

    /** 使用QImage自带的转化方法
    * QImage newImage = originImage.convertToFormat(QImage::Format_Grayscale8);
    */

    QPixmap pix;
    if(imageInfo.image.size() != width*height*2){
        SDKLog::logE("FrameWidget show pic failed! image size = ", imageInfo.image.size());
    }else{
        //获取真实高度、预测高度值
        unsigned int realHeight = imageInfo.realHeight;
        unsigned int evaHeight = imageInfo.evaHeight;
        QByteArray imageData = imageInfo.image;
    
        SDKLog::logD("realHeight = ", realHeight);
        SDKLog::logD("evaHeight = ", evaHeight);

        //将16位灰度图转化为8位显示
        QByteArray eightImg;
        eightImg.resize(imageData.size() / 2);
        eightImg = du.convert16ImgTo8Img(imageData, isEnhanceImg);

        //直方图均衡
        //eightImg = du.histgram(eightImg);
        QImage image = QImage((const unsigned char*)eightImg.data(), width, height, format);
        //画框
        image = du.editImage(image, evaHeight, realHeight);
        pix = QPixmap::fromImage(image);
    }

    return pix;
}

/*
* @brief FrameWidget::showStatus 在无需显示图片时，显示目前的状态
* @param status 状态码
*/
void FrameWidget::setStatus(int status){
    statusNow = status;
    update();
}

/**
 * @brief FrameWidget::setImgEnhance 设置图像增强开关
 * @param status 0 关闭图像增强
 *               1 开启图像增强
 */
void FrameWidget::setImgEnhance(int status){
    isEnhanceImg = status;
}

/***************************** 帧动画代码，暂未使用 *****************************/
/**
* 可控制帧率，暂时未想好实现方法
*/
void FrameWidget::init(const int frameRate){
    timer = new QTimer(this);
    timer->setInterval(frameRate);
    isTimeOutflag = false;
    connect(timer, SIGNAL(timeout()), this, SLOT(updateClockwise()));
}

/**
* 设置PixMap序列，按照传入的帧率显示帧动画
* @param _msec: 帧动画的帧率
*/
void FrameWidget::setAnimation(const int _msec) {
    currentIndex = 0;

    if (!pixList.empty()) {
        pixList.clear();
    }
    else {
        /*  顺时针动画关联  */
        timer = new QTimer(this);
        timer->setInterval(_msec);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateClockwise()));
    }
    //pixList.append(getGrayscaleImg(dataUtils::picMap.value(i), PIC_WIDTH, PIC_WIDTH, QImage::Format_Grayscale16));

    currentPix = pixList.at(0);

    update();

}

/**
* 开始帧动画
*/
void FrameWidget::startClockwise() {
    currentIndex = 0;
    timer->start();
}

/**
* 停止帧动画
*/
void FrameWidget::stop() {
    if(timer->isActive()) {
        timer->stop();
    }
    currentIndex = 0;
    currentPix = pixList.at(0);
    update();
}

/**
* timer的timeout触发槽函数，在这里进行PixMap的更新
*/
void FrameWidget::updateClockwise(void) {
    isTimeOutflag = true;
    do {
        if (currentIndex >= 0) {
            /*  更新帧  */
            currentPix = pixList.at(currentIndex);
            update();

            /*  判断帧数  */
            if (currentIndex >= (6 - 1)) {
                if(isLoop_) {
                    currentIndex = 0;
                    return;
                }
                break;
            }

            /*  跳帧  */
            ++currentIndex;
            return;
        }
        else {
            SDKLog::log("wrong subffix, index = ", currentIndex);
        }
    } while(false);

    timer->stop();
    currentIndex = 0;
    emit clockwiseFinished();
}


