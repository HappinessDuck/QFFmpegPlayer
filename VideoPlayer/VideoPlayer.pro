QT       += core gui
QT += concurrent
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    controlview.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    controlview.h \
    mainwindow.h

FORMS += \
    controlview.ui \
    mainwindow.ui

TRANSLATIONS += \
    VideoPlayer_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


include($$PWD/module/FFmpegServer/FFmpegServer.pri)

contains(QT_ARCH, i386) {
    message("32-bit")
    DESTDIR = $${PWD}/bin32
} else {
    message("64-bit")
    DESTDIR = $${PWD}/bin64
}

#程序版本
VERSION  = 2024.10.01
#程序图标
#RC_ICONS = main.ico
#产品名称
QMAKE_TARGET_PRODUCT = VideoPlayer
#版权所有
QMAKE_TARGET_COPYRIGHT = wqj
#文件说明
QMAKE_TARGET_DESCRIPTION = QQ: 1107097641


#禁用qdebug打印输出
#DEFINES += QT_NO_DEBUG_OUTPUT
