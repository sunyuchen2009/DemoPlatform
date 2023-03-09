#include "numKeyboard.h"

QString st_letter[11]={"0","1","2","3","4","5","6","7","8","9","."};

NumKeyboard::NumKeyboard(QWidget *parent) :
    QDialog(parent)
{
    QGridLayout *layout = new QGridLayout();
    layout->setSpacing(30);

    QButtonGroup *button_group = new QButtonGroup(this);

    lineEdit = new QLineEdit(this);
    //lineEdit->setText(QString()); //QString的构造函数会无限复制导致栈溢出
    layout->addWidget(lineEdit,0,0,1,5);

    num0Button = new QPushButton(this);
    num0Button->setFixedSize(150, 50);
    num0Button->setText("0");
    button_group->addButton(num0Button, BTN_NUM_0);
    layout->addWidget(num0Button,4,0,1,2);

    num1Button = new QPushButton(this);
    num1Button->setFixedSize(50, 50);
    num1Button->setText("1");
    button_group->addButton(num1Button, BTN_NUM_1);
    layout->addWidget(num1Button,1,0,1,1);

    num2Button = new QPushButton(this);
    num2Button->setFixedSize(50, 50);
    num2Button->setText("2");
    button_group->addButton(num2Button, BTN_NUM_2);
    layout->addWidget(num2Button,1,1,1,1);

    num3Button = new QPushButton(this);
    num3Button->setFixedSize(50, 50);
    num3Button->setText("3");
    button_group->addButton(num3Button, BTN_NUM_3);
    layout->addWidget(num3Button,1,2,1,1);

    num4Button = new QPushButton(this);
    num4Button->setFixedSize(50, 50);
    num4Button->setText("4");
    button_group->addButton(num4Button, BTN_NUM_4);
    layout->addWidget(num4Button,2,0,1,1);

    num5Button = new QPushButton(this);
    num5Button->setFixedSize(50, 50);
    num5Button->setText("5");
    button_group->addButton(num5Button, BTN_NUM_5);
    layout->addWidget(num5Button,2,1,1,1);

    num6Button = new QPushButton(this);
    num6Button->setFixedSize(50, 50);
    num6Button->setText("6");
    button_group->addButton(num6Button, BTN_NUM_6);
    layout->addWidget(num6Button,2,2,1,1);

    num7Button = new QPushButton(this);
    num7Button->setFixedSize(50, 50);
    num7Button->setText("7");
    button_group->addButton(num7Button, BTN_NUM_7);
    layout->addWidget(num7Button,3,0,1,1);

    num8Button = new QPushButton(this);
    num8Button->setFixedSize(50, 50);
    num8Button->setText("8");
    button_group->addButton(num8Button, BTN_NUM_8);
    layout->addWidget(num8Button,3,1,1,1);

    num9Button = new QPushButton(this);
    num9Button->setFixedSize(50, 50);
    num9Button->setText("9");
    button_group->addButton(num9Button, BTN_NUM_9);
    layout->addWidget(num9Button,3,2,1,1);

    dotButton = new QPushButton(this);
    dotButton->setFixedSize(50, 50);
    dotButton->setText(".");
    button_group->addButton(dotButton, BTN_POINT);
    layout->addWidget(dotButton,4,2,1,1);

    cancelButton = new QPushButton(this);
    cancelButton->setFixedSize(120, 50);
    cancelButton->setText("取消");
    button_group->addButton(cancelButton, BTN_CANCEL);
    layout->addWidget(cancelButton,1,3,1,2);

    backspaceButton = new QPushButton(this);
    backspaceButton->setFixedSize(120, 50);
    backspaceButton->setText("<<");
    button_group->addButton(backspaceButton, BTN_BKSP);
    layout->addWidget(backspaceButton,2,3,1,2);

    leftButton = new QPushButton(this);
    leftButton->setFixedSize(30, 50);
    leftButton->setText("<-");
    button_group->addButton(leftButton, BTN_LEFT);
    layout->addWidget(leftButton,3,3,1,1);

    rightButton = new QPushButton(this);
    rightButton->setFixedSize(30, 50);
    rightButton->setText("->");
    button_group->addButton(rightButton, BTN_RIGHT);
    layout->addWidget(rightButton,3,4,1,1);

    okButton = new QPushButton(this);
    okButton->setFixedSize(120, 50);
    okButton->setText("确定");
     button_group->addButton(okButton, BTN_ENTER);
    layout->addWidget(okButton,4,3,1,2);

    //连接button_group的点击信号，和本对象的buttonClickResponse函数，传递参数为按钮号
    connect(button_group,SIGNAL(buttonReleased(int)),SLOT(buttonClickResponse(int)));

    //为所有按钮控件添加objectName，提供给QSS样式使用
    //QList<QPushButton*> btnList = this->findChildren<QPushButton*>();
    //for(int i = 0; i < btnList.size(); i++)
    //{
    //    QPushButton* btn = btnList.at(i);
    //    btn->setObjectName("keyboard");
    //}

    setLayout(layout);
    //SDKLog::log("numKeyboard initialed");
}

NumKeyboard::~NumKeyboard()
{

}

void NumKeyboard::changeEvent(QEvent *e)
{
    //SDKLog::logE("eventFilter event type = ", e->type());
    QWidget *pActiveWindow = QApplication::activeWindow();
    NumKeyboard *numDialog = dynamic_cast<NumKeyboard*>(pActiveWindow);
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::ActivationChange:
            if(numDialog && numDialog == this){
                
            }else{
                this->close();
                valid = false;
            }
            break;
        default:
            break;
    }
}

bool NumKeyboard::eventFilter(QObject *obj, QEvent *event){
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (obj == lineEdit)
        {
            if(keyEvent->key() >= 0x20 && keyEvent->key()<= 0x0ff)  //屏蔽所有按键输入
                return true;
            else
                return false;
        }
        else
        {
            return false;
        }
    }
    else
    {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

//***********************按键*****************************//
void NumKeyboard::buttonClickResponse(int gemfield){
    int idx = -1; //光标的位置索引
    switch (gemfield) {
        case BTN_NUM_0:
        case BTN_NUM_1:
        case BTN_NUM_2:
        case BTN_NUM_3:
        case BTN_NUM_4:
        case BTN_NUM_5:
        case BTN_NUM_6:
        case BTN_NUM_7:
        case BTN_NUM_8:
        case BTN_NUM_9:
        case BTN_POINT:
            idx = lineEdit->cursorPosition();
            //字符串的存储有引用计数，当一个 QString 对象被复制为另一个 QString 对象时，它们实际上指向相同的存储空间，仅仅是增加一个引用计数
            strContent.insert(idx, st_letter[gemfield]);  //插入字符串str在给定的索引位置对这个字符串,并返回一个引用。 //数据存入strContent
            lineEdit->setText(strContent); //strContent中的内容显示在lineEdit
            lineEdit->setCursorPosition(idx+1); //设置光标的位置 ??不设置好像也可以使用设定光标的位置
            lineEdit->setFocus();  //存在光标

            //qDebug()<<"strContent"<<strContent;
            //qDebug()<<"idx"<<idx;
            break;

        case BTN_CANCEL:
            this->close();
            valid = false;
            break;
        case BTN_BKSP:
            idx = lineEdit->cursorPosition();
            if(idx == 0){
                lineEdit->setCursorPosition(idx);
                lineEdit->setFocus();
                return;
            }
            strContent.remove(idx-1,1);
            lineEdit->setText(strContent);
            lineEdit->setCursorPosition(idx-1);
            lineEdit->setFocus();
            break;
        case BTN_LEFT:
            idx = lineEdit->cursorPosition();
            if(idx == 0){
                lineEdit->setCursorPosition(idx);
                lineEdit->setFocus();
                return;
            }
            lineEdit->setCursorPosition(idx-1);
            lineEdit->setFocus();
            break;
        case BTN_RIGHT:
            idx = lineEdit->cursorPosition();
            if(idx == strContent.length()){ //返回此字符串的字符数
                lineEdit->setCursorPosition(idx);
                lineEdit->setFocus();
                return;
            }
            lineEdit->setCursorPosition(idx+1);
            lineEdit->setFocus();
            break;
        case BTN_ENTER:
            if(strContent.toUInt() > 0 && strContent.toUInt() < 49){
                this->close();
                valid = true;
            }else{
                CToast::ShowText("Inut invalid! Please input 1-48", 2000, 0.5, 0.5, this);
            }
            
            break;
        default:
            this->close();
            valid = false;
            break;
    }
}

void NumKeyboard::setText(QString str){ //设置文本内容
    strContent = str;
    lineEdit->setText(strContent);
}

QString NumKeyboard::getText(){ //获取内容
    return strContent;
}
