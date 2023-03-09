#ifndef KEYBOARDLINEEDIT_H
#define KEYBOARDLINEEDIT_H

#include <QLineEdit>
#include <QMouseEvent>

#include "SDKLog.h"
#include "numKeyboard.h"

class KeyboardLineEdit:public QLineEdit{
 Q_OBJECT
public:
    explicit KeyboardLineEdit(QWidget *parent = 0);
     ~KeyboardLineEdit();
protected:
    //添加并重写鼠标点击事件函数
    virtual void mousePressEvent(QMouseEvent *e);
signals:
    //点击信号函数
    void clicked();
private:
    NumKeyboard *numkeyboard;
};

#endif // KEYBOARDLINEEDIT_H
