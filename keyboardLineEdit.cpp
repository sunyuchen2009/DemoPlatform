#include "keyboardLineEdit.h"

KeyboardLineEdit::KeyboardLineEdit(QWidget *parent):QLineEdit(parent){
    numkeyboard = new NumKeyboard(this);
}
KeyboardLineEdit::~KeyboardLineEdit(){

}

//重写mousePressEvent事件,检测事件类型是不是点击了鼠标左键
void KeyboardLineEdit::mousePressEvent(QMouseEvent *e) {
    //如果单击了就触发clicked信号
    //if (e->button() == Qt::LeftButton) {
    //    //触发clicked信号
    //    emit clicked();
    //}
    if(e->button() == Qt::LeftButton){
        //SDKLog::log("EditText clicked!");
        numkeyboard->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        numkeyboard->setStyleSheet("border:2px solid white;");
        numkeyboard->setText(this->text());  //当前的文本框的内容，this->指向的是SoftKeyLineEdit
        numkeyboard->show();
        numkeyboard->exec();
        if(numkeyboard->valid){
            this->setText(numkeyboard->getText());
        }
    }
    //将该事件传给父类处理
    // QLineEdit::mousePressEvent(e);
}
