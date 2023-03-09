QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    SDKLog.cpp \
    dataUtils.cpp \
    eimController.cpp \
    frameWidget.cpp \
    itemDelegate.cpp \
    keyboardLineEdit.cpp \
    main.cpp \
    mainwindow.cpp \
    numKeyboard.cpp \
    toast.cpp \
    writeResultFileThread.cpp \
    qcustomplot.cpp

HEADERS += \
    SDKLog.h \
    dataUtils.h \
    defines.h \
    eimController.h \
    frameWidget.h \
    itemDef.h \
    itemDelegate.h \
    keyboardLineEdit.h \
    mainwindow.h \
    numKeyboard.h \
    toast.h \
    writeResultFileThread.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
