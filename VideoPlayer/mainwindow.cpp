#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QStackedLayout>
#include <QDebug>
#include <QMutex>
#include <QtConcurrent>
#include <QFuture>
#include <QTime>
#include <QSlider>
#include <QLabel>
#include <QFileDialog>
#include <QCoreApplication>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("VideoPlayer");

    setMinimumWidth(400);
    setMinimumHeight(400);
    //    setMaximumWidth(1920);
    //    setMaximumHeight(1080);
    ui->gridLayout->setContentsMargins(0,0,0,0);
    setMouseTracking(true);

    //play list init
    ui->dockWidget_3->setMinimumWidth(200);
    ui->dockWidget_3->setWindowTitle("play list");

    QPushButton *btn_open = new QPushButton;
    btn_open->setText("open file");
    ui->dockWidget_3->setWidget(btn_open);
    addDockWidget(Qt::RightDockWidgetArea, ui->dockWidget_3);

    connect(btn_open, &QPushButton::clicked, this, &MainWindow::openFile);

    //播放列表--双击播放视频
    listWidget = new QListWidget();
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::handleItemDoubleClicked);
    //    QObject::connect(listWidget, &QListWidget::itemDoubleClicked, [&](QListWidgetItem *item){
    //        qDebug() << "Double clicked: " << item->text();
    //    });


    m_FFmpegServer = new FFmpegServer(this);
    connectControl();

    //init
    isPause = false;
    isSoundOff = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setProgress(QString labelName, QString value)
{
    QLabel *child = ui->controlWidget->findChild<QLabel*>(labelName);
    if(child)
    {
        child->setText(value);
    }
}

QString MainWindow::intToTimeFormat(int input)
{
    int hours = input / 60;
    int minutes = input % 60;
    QTime time(hours, minutes);
    return time.toString("hh:mm");
}

void MainWindow::connectControl()
{
    //bind QPushButton
    const QStringList buttonNames = {"pushButton_list", "pushButton_start",
                                     "pushButton_previous","pushButton_next",
                                     "pushButton_soundOff",
                                     "pushButton_forwardSecond","pushButton_backwardSecond"
                                    };
    for (const QString &buttonName : buttonNames) {
        QPushButton *childButton = ui->controlWidget->findChild<QPushButton*>(buttonName);
        if (childButton)
        {
            if (buttonName == "pushButton_list")
            {
                setIcon(buttonName, 33);//列表图标
                connect(childButton, &QPushButton::clicked, this, &MainWindow::showPlayListView);
            } else if(buttonName == "pushButton_start")
            {
                setIcon(buttonName, 61);//播放图标
                connect(childButton, &QPushButton::clicked, this, &MainWindow::pauseVideo);
            }else if(buttonName == "pushButton_soundOff")
            {
                setIcon(buttonName, 68);//声音图标
                connect(childButton, &QPushButton::clicked, this, &MainWindow::soundOff);
            }else if(buttonName == "pushButton_previous")
            {
                setIcon(buttonName, 65);//上一集
            }else if(buttonName == "pushButton_next")
            {
                setIcon(buttonName, 64);//下一集
            }
            else if(buttonName == "pushButton_forwardSecond")
            {
                setIcon(buttonName, 67);//向前s
                //                connect(childButton, &QPushButton::clicked, this, &MainWindow::soundOff);
            }else if(buttonName == "pushButton_backwardSecond")
            {
                setIcon(buttonName, 66);//向后s
                //                connect(childButton, &QPushButton::clicked, this, &MainWindow::soundOff);
            }
        }
    }

    const QStringList sliderNames = {"horizontalSlider_progress", "horizontalSlider_volume"};

    //bind QSlider
    for (const QString &sliderName : sliderNames) {
        QSlider *child = ui->controlWidget->findChild<QSlider*>(sliderName);
        if (child) {
            if (sliderName == "horizontalSlider_progress") {
                connect(child, &QSlider::valueChanged, m_FFmpegServer, &FFmpegServer::setProgress);
            } else if (sliderName == "horizontalSlider_volume") {
                child->setMinimum(0);
                child->setMaximum(100);
                connect(child, &QSlider::valueChanged, m_FFmpegServer, &FFmpegServer::setVolume);
            }
        }
    }

    const QStringList labelNames = {"label_allDuration", "label_currentDuration"};

    //bind QSlider
    for (const QString &name : labelNames) {
        QLabel *child = ui->controlWidget->findChild<QLabel*>(name);
        if (child) {
            child->setAlignment(Qt::AlignCenter);
            child->setText("00:00");
        }
    }

    connect(m_FFmpegServer, &FFmpegServer::setImage_signal, this, &MainWindow::showImage, Qt::QueuedConnection);

}

QList<QString> MainWindow::getFilesInDirectory()
{
    QList<QString> mp4Files;

    QString currentPath = QCoreApplication::applicationDirPath();
    QDir directory(currentPath);

    videoPath = QFileDialog::getExistingDirectory(nullptr, "Select Directory", currentPath);

    if (!directory.exists()) {
        qDebug() << "Directory does not exist.";
        return mp4Files;
    }

    QStringList filters;
    filters << "*.mp4";
    QStringList fileList = directory.entryList(filters, QDir::Files);

    foreach (const QString &file, fileList) {
        mp4Files.append(file);
    }

    return mp4Files;
}

void MainWindow::playVideo(QString path)
{
    m_FFmpegServer->clear();
    m_FFmpegServer->setPlayUrl(path);

    int ret = m_FFmpegServer->initInput();
    if(ret != 0)
    {
        qDebug()<<"initInput failed";
        return;
    }

    //set video msg
    curPos = 0;
    allPos = m_FFmpegServer->getLength();
    setProgress("label_allDuration", intToTimeFormat(allPos));
    setProgress("label_currentDuration", "00:00");

    m_FFmpegServer->start();

    setIcon("pushButton_start", 63);//播放图标
}

void MainWindow::showPlayListView()
{
    if (ui->dockWidget_3->isHidden())
    {
        ui->dockWidget_3->show();
    }else{
        ui->dockWidget_3->hide();
    }
}

void MainWindow::showImage(QImage image)
{
    //    m_image = image.copy();
    //    update(); // 重新绘制窗口
    QPixmap pix = QPixmap::fromImage(image);
    ui->label->setScaledContents(true);
    ui->label->setPixmap(pix.scaled(ui->label->size(), Qt::KeepAspectRatio));
}

void MainWindow::pauseVideo()
{
    m_FFmpegServer->setPause();
    if(isPause)
    {
        setIcon("pushButton_start", 63);
    }else{
        setIcon("pushButton_start", 61);
    }
    isPause = !isPause;

}

void MainWindow::soundOff()
{
    m_FFmpegServer->setSoundOff();
    //图标切换为静音和正常
    if(isSoundOff)
    {
        setIcon("pushButton_soundOff", 68);//声音图标
    }else{
        setIcon("pushButton_soundOff", 69);//声音图标
    }
    isSoundOff = !isSoundOff;
}

void MainWindow::openFile()
{
    QString videoName = "C:/Users/11070/Videos/1.mp4";
    playVideo(videoName);
#if 0
    QList<QString> fileMsg = getFilesInDirectory();
    qDebug()<<fileMsg;

    for (const QString &file : fileMsg)
    {
        listWidget->addItem(file);
    }
    ui->dockWidget_3->setWidget(listWidget);
#endif
}

void MainWindow::handleItemDoubleClicked(QListWidgetItem *item)
{
    //init video
    QString videoName = QDir::cleanPath(QDir(videoPath).filePath(item->text()));
    qDebug() << "Double clicked: " << videoName;
    playVideo(videoName);

}

void MainWindow::setIcon(QString btnName, int flag)
{
    QPushButton *childButton = ui->controlWidget->findChild<QPushButton*>(btnName);
    if (childButton)
    {
        QStyle *style = QApplication::style();
        QIcon icon;
        icon = style->standardIcon((QStyle::StandardPixmap)flag);
        childButton->setIcon(icon);
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //    // 获取鼠标的当前位置
    //    int x = event->x();
    //    int y = event->y();

    //    // 在控制台输出鼠标的位置
    //    //    qDebug() << "Mouse moved to: (" << x << ", " << y << ")";
    //    if (event->pos().x() > 0 && event->pos().y() > 0 && event->pos().x() < width() && event->pos().y() < height())
    //    {
    //        setHiddenForAllWidgets(ui->verticalLayout, false);
    //    } else
    //    {
    //        setHiddenForAllWidgets(ui->verticalLayout, true);
    //    }

}

void MainWindow::enterEvent(QEvent *event)
{
    ui->controlWidget->show();
}

void MainWindow::leaveEvent(QEvent *event)
{
    ui->controlWidget->hide();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    clear();


    // 调用父类的closeEvent以确保正常关闭窗口
//    QMainWindow::closeEvent(event);
}


void MainWindow::clear()
{
    //关闭线程
    if (m_FFmpegServer->isRunning())
    {
        m_FFmpegServer->stopPlay();
        m_FFmpegServer->quit();
        m_FFmpegServer->wait(150);
        m_FFmpegServer->clear();
    }
}

