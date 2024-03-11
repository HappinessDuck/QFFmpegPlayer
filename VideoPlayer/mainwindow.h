#ifndef MAINWINDOW_H
#define MAINWINDOW_H
/**************************************************\
 * 类名: MainWindow
 * 描述: 主界面
 * 信号：
 * 函数：
 * 备注：
 * 作者：wqj  QQ:1107097641
 * 博客地址：
 *      日期          版本号                描述
 * 2024年03年11日     v1.0.0         主窗口，添加视频列表窗口
\**************************************************/

#include "FFmpegServer.h"
#include <QMainWindow>
#include <QPushButton>
#include <QListWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setProgress(QString labelName, QString value);
    QString intToTimeFormat(int input);

private:
    Ui::MainWindow *ui;
    FFmpegServer *m_FFmpegServer;

    bool isPause;
    bool isSoundOff;

    void connectControl();
    int curPos;             //当前进度
    int allPos;             //总进度



    QPushButton *btn_open;
    QListWidget *listWidget;

    QString videoPath;
    QList<QString> getFilesInDirectory();

private slots:
    //显示播放列表
    void showPlayListView();

    void playVideo(QString path);
    void showImage(QImage image);
    void pauseVideo();      //暂停并继续播放
    void soundOff();        //静音和恢复声音


    void openFile();        //获取路径所有播放视频
    void handleItemDoubleClicked(QListWidgetItem *item);


    void changeButtonStatus();


signals:
//    void changePos_signal(int value);
//    void changeVolume_signal(int value);
//    void play();
//    void nextVideo();
//    void previos();



protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
};
#endif // MAINWINDOW_H
