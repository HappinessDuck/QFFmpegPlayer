#include "controlview.h"
#include "ui_controlview.h"

ControlView::ControlView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlView)
{
    ui->setupUi(this);



//    //    setStyleSheet("background-color: rgba(0, 0, 0, 0);");

//    setWindowFlags(Qt::FramelessWindowHint); // 去掉标题栏

//    //    setAttribute(Qt::WA_TranslucentBackground); // 设置背景为透明
//    // 追踪鼠标 用于播放时隐藏鼠标
//    setMouseTracking(true);
////    setPlayListView();

//    m_FFmpegServer = new FFmpegServer();
//    //绑定控制
//    connectControl();

}

ControlView::~ControlView()
{
    delete ui;
}


