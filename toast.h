#ifndef TOAST_H
#define TOAST_H

#include <QDialog>
#include <QMouseEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QDebug>
#include <QScreen>
#include <QMovie>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

/*
 * CToast : 仿Android中Toast控件，用于弹出任意Widget，
 *          常见用法如弹出一个文字提示，图片，GIF等，并于特定时间后自动隐藏
 *
 * 类中提供了三个静态函数分别用于显示文本，图片和GIF。如有其他需要可以参照这些静态函数自行修改。
*/

class CToast : public QDialog
{
    Q_OBJECT
public:
    CToast(QWidget *parent = nullptr);
    ~CToast();

    /*
     * SetWidget:设置要展示的Widget窗口，默认为nullptr
    */
    void SetWidget(QWidget *widget);

    /*
     * SetDisplayTime:设置 CToast 的显示时间，超过这个时间，窗口自动隐藏，单位：毫秒
     *
     * 默认为 -1 ，也就是不会自动消失
     *
     * 如果设置了正值的显示时间，隐藏窗口的时候，会自动根据SetAnimation的值来决定是否开启隐藏动画
    */
    void SetDisplayTime(int mSecs);

    /*
     * SetAnimation:显示和隐藏窗口时，是否采用动画，而不是直接显示和消失
     *
     * 默认为 true ，也就是使用动画，显示时，应该调用StartShowAnimation()
    */
    void SetAnimation(bool show);

    /*
     * SetDeleteWhenHide:设置窗口隐藏时是否自动删除CToast,默认为false
     *
     * 这个方法，主要是用于静态函数中的，此时用于不会获取CToast的指针，所以需要自动删除掉
     * 如果用户不是使用静态函数，而是需要自已 new 出一个 CToast 的，则最好不要用这个函数
    */
    void SetDeleteWhenHide(bool deleteWhenHide);

    /*
    * MoveToPercentage:将CToast的中点移动到指定的位置，参数hPer和vPer表示百分比。
    *
    * 如果CToast的parent为一个Widget，那么它的位置是相对于parent widget的
    * 如果CToast的parent为nullptr，那么它的位置是相对于电脑的主屏幕的
    *
    * 如果取值为0~1.0，那么CToast会自动移动到指定的百分比点，如果其中有任意一个设置为负数，
    * 那么，不会自动移动，此时需要用户去调用 QWidget::move()来指定位置。
    *
    * MoveToCenter 即为 MoveToPercentage(0.5,0.5);
    *
    * 默认会调用MoveToPercentage(-1,-1),也就是不开启自动移动
    */
    void MoveToCenter();
    void MoveToPercentage(qreal hPer,qreal vPer);

public slots:
    void StartShowAnimation();
    void StartHideAnimation();

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
private:
    void Init();
    void InitAnimation();
    void SetDelaySeconds(int mSecs);
    void MovePostion();

    QWidget            *m_pCentralWidget;
    qreal               m_dHPer,m_dVPer;
    int                 m_nDisplayTimeByMSecs;
    bool                m_bShowAnimation;
    bool                m_bDeleteWhenHide;
    QPropertyAnimation *m_pHideAnimation;
    QPropertyAnimation *m_pShowAnimation;

public:
    static void ShowText(const QString &text,int displayTimeByMSec = 5000,
                         double hPercentage = 0.5,double vPercentage = 0.8,QWidget *parent = nullptr)
    {
        QFont iFont;
        iFont.setPixelSize(24);

        QLabel *pLabel = new QLabel;
        pLabel->setFrameStyle(QFrame::StyledPanel|QFrame::Plain);
        pLabel->setAlignment(Qt::AlignCenter);
        pLabel->setFont(iFont);
        pLabel->setWordWrap(true);
        pLabel->setText(text);

        QString qss = "background-color:transparent;"
                      "border:none;"
                      "color:#27408B;";

        pLabel->setStyleSheet(qss);


        CToast *iToast = new CToast(parent);
        QFontMetrics metrics(iFont);
        QRect rect = metrics.boundingRect(QRect(0,0,pLabel->maximumWidth(),pLabel->maximumHeight()),
            Qt::AlignCenter|Qt::TextWordWrap,text);
        iToast->resize(rect.size()+QSize(metrics.width("A"),0));

        iToast->SetWidget(pLabel);
        iToast->SetDisplayTime(displayTimeByMSec);
        iToast->SetDeleteWhenHide(true);
        iToast->MoveToPercentage(hPercentage,vPercentage);
        iToast->StartShowAnimation();
    }

    static void ShowPixmap(const QPixmap &pixmap,int displayTimeByMSec = 5000,
                           double hPercentage = 0.5,double vPercentage = 0.8,QWidget *parent = nullptr)
    {
        if(pixmap.isNull())
        {
            ShowText(u8"pixmap为NULL，无法显示图片!",displayTimeByMSec,hPercentage,vPercentage,parent);
            return;
        }

        QLabel *pLabel = new QLabel;
        pLabel->setFrameStyle(QFrame::Panel);
        pLabel->setAlignment(Qt::AlignCenter);
        pLabel->setPixmap(pixmap);


        CToast *iToast = new CToast(parent);
        iToast->resize(pixmap.size());

        iToast->SetWidget(pLabel);
        iToast->SetDisplayTime(displayTimeByMSec);
        iToast->SetDeleteWhenHide(true);
        iToast->MoveToPercentage(hPercentage,vPercentage);
        iToast->StartShowAnimation();
    }

    static void ShowMoive(QMovie *movie,double hPercentage = 0.5,double vPercentage = 0.8,
                          QWidget *parent = nullptr)
    {
        if((movie == nullptr) || !movie->isValid())
        {
            ShowText(u8"movie为nullptr或无效，无法显示Gif!",5000,hPercentage,vPercentage,parent);
            return;
        }

        QLabel *pLabel = new QLabel;
        pLabel->setFrameStyle(QFrame::Panel);
        pLabel->setAlignment(Qt::AlignCenter);
        pLabel->setMovie(movie);


        CToast *iToast = new CToast(parent);
        iToast->resize(QPixmap(movie->fileName()).size());
        iToast->SetWidget(pLabel);
        iToast->SetDisplayTime(-1);
        iToast->SetDeleteWhenHide(true);
        iToast->MoveToPercentage(hPercentage,vPercentage);
        iToast->StartShowAnimation();
        movie->start();

        if(iToast->m_bShowAnimation)
        {
            QObject::connect(movie,SIGNAL(finished()),iToast,SLOT(StartHideAnimation()));
        }
        else
        {
            QObject::connect(movie,SIGNAL(finished()),iToast,SLOT(hide()));
        }
    }
};

#endif // TOAST_H
