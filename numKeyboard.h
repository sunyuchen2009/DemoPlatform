#ifndef NUMKEYBOARD_H
#define NUMKEYBOARD_H

#include <QDialog>
#include <QtDebug>
#include <QKeyEvent>
#include <QEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>
#include <QGridLayout>
#include <QButtonGroup>
#include "SDKLog.h"
#include "toast.h"

#define BTN_NUM_0  0
#define BTN_NUM_1  1
#define BTN_NUM_2  2
#define BTN_NUM_3  3
#define BTN_NUM_4  4
#define BTN_NUM_5  5
#define BTN_NUM_6  6
#define BTN_NUM_7  7
#define BTN_NUM_8  8
#define BTN_NUM_9  9
#define BTN_POINT  10
#define BTN_CANCEL 11
#define BTN_BKSP   12
#define BTN_LEFT   13
#define BTN_RIGHT  14
#define BTN_ENTER  15

class NumKeyboard : public QDialog
{
    Q_OBJECT

public:
    explicit NumKeyboard(QWidget *parent = 0);
    ~NumKeyboard();
    bool valid;

    void setText(QString str);
    QString getText();

    QPushButton *num6Button;
    QPushButton *backspaceButton;
    QPushButton *num4Button;
    QPushButton *okButton;
    QPushButton *leftButton;
    QPushButton *num1Button;
    QPushButton *cancelButton;
    QPushButton *rightButton;
    QPushButton *num9Button;
    QPushButton *num8Button;
    QPushButton *num2Button;
    QPushButton *num7Button;
    QPushButton *dotButton;
    QPushButton *num3Button;
    QPushButton *num0Button;
    QPushButton *num5Button;
    QPushButton *signButton;
    QLineEdit *lineEdit;

protected:
    void changeEvent(QEvent *e);
//    void keyPressEvent(QKeyEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void buttonClickResponse(int gemfield);

private:
    QString strContent;
};

#endif // NUMKEYBOARD_H
