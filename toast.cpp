#include "toast.h"

CToast::CToast(QWidget *parent)
    :QDialog (parent)
{
    m_pCentralWidget = nullptr;
    m_pHideAnimation = nullptr;
    m_pShowAnimation = nullptr;
    m_bShowAnimation = true;
    m_bDeleteWhenHide = false;
    m_nDisplayTimeByMSecs = -1;
    m_dHPer = -1;
    m_dVPer = -1;

    Init();
    InitAnimation();
}

CToast::~CToast()
{
    if(m_pHideAnimation)
        m_pHideAnimation->deleteLater();
    if(m_pShowAnimation)
        m_pShowAnimation->deleteLater();
    if(m_pCentralWidget)
    {
        m_pCentralWidget->setParent(nullptr);
        m_pCentralWidget->deleteLater();
    }
}

void CToast::SetWidget(QWidget *widget)
{
    if(nullptr == widget)
        return;

    if(m_pCentralWidget)
    {
        m_pCentralWidget->setParent(nullptr);
        m_pCentralWidget->deleteLater();
    }

    QVBoxLayout *iMainLayout = new QVBoxLayout;
    iMainLayout->setMargin(0);
    iMainLayout->setSpacing(0);

    m_pCentralWidget = widget;
    iMainLayout->addWidget(m_pCentralWidget);

    this->setLayout(iMainLayout);
}

void CToast::SetDisplayTime(int mSecs)
{
    m_nDisplayTimeByMSecs = mSecs;
}

void CToast::SetAnimation(bool show)
{
    m_bShowAnimation = show;
}

void CToast::SetDeleteWhenHide(bool deleteWhenHide)
{
    m_bDeleteWhenHide = deleteWhenHide;
}

void CToast::MoveToCenter()
{
    m_dHPer = 0.5;
    m_dVPer = 0.5;
}

void CToast::MoveToPercentage(qreal hPer, qreal vPer)
{
    m_dHPer = (hPer < 0) ? -1 : ((hPer > 1.0) ? 1.0 : hPer);
    m_dVPer = (vPer < 0) ? -1 : ((vPer > 1.0) ? 1.0 : vPer);
}

void CToast::Init()
{
    if(nullptr == this->parentWidget())
    {
       this->setWindowFlags(Qt::FramelessWindowHint|Qt::Tool|Qt::WindowStaysOnTopHint);
    }
    else
    {
        this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    }

    this->setAttribute(Qt::WA_AlwaysStackOnTop,true);
    this->setAttribute(Qt::WA_TranslucentBackground,true);
}

void CToast::InitAnimation()
{
    QGraphicsOpacityEffect *pOpacityEffect = new QGraphicsOpacityEffect(this);
    pOpacityEffect->setOpacity(0.0);
    this->setGraphicsEffect(pOpacityEffect);

    m_pHideAnimation = new QPropertyAnimation(pOpacityEffect, "opacity",this);
    m_pHideAnimation->setEasingCurve(QEasingCurve::Linear);
    m_pHideAnimation->setDuration(1200);
    m_pHideAnimation->setEndValue(0);

    m_pShowAnimation = new QPropertyAnimation(pOpacityEffect, "opacity",this);
    m_pShowAnimation->setEasingCurve(QEasingCurve::Linear);
    m_pShowAnimation->setDuration(1200);
    m_pShowAnimation->setEndValue(1.0);

    connect(m_pHideAnimation,SIGNAL(finished()),this,SLOT(hide()));
}

void CToast::SetDelaySeconds(int mSecs)
{
    if(mSecs > 0)
    {
        if(m_bShowAnimation)
        {
            QTimer::singleShot(mSecs,this,SLOT(StartHideAnimation()));
        }
        else
        {
            QTimer::singleShot(mSecs,this,SLOT(hide()));
        }
    }
}

void CToast::MovePostion()
{
    qreal hPer = m_dHPer;
    qreal vPer = m_dVPer;

    if(hPer == -1 || vPer == -1)
        return;

    int pWidth,pHeight;
    QWidget *parentWidget = this->parentWidget();
    if(nullptr != parentWidget)
    {
        pWidth = this->parentWidget()->width();
        pHeight = this->parentWidget()->height();
    }
    else
    {
        pWidth = QApplication::primaryScreen()->size().width();
        pHeight = QApplication::primaryScreen()->size().height();
    }

    this->move(pWidth*hPer - this->width()*0.5,pHeight*vPer - this->height()*0.5);
}

void CToast::StartShowAnimation()
{
    this->setVisible(true);
    m_pShowAnimation->start();
}

void CToast::StartHideAnimation()
{
    m_pHideAnimation->start();
}

void CToast::showEvent(QShowEvent *event)
{
    MovePostion();
    QDialog::showEvent(event);
    this->SetDelaySeconds(m_nDisplayTimeByMSecs);
}

void CToast::hideEvent(QHideEvent *event)
{
    QDialog::hideEvent(event);
    if(m_bDeleteWhenHide)
    {
        this->deleteLater();
    }
}
