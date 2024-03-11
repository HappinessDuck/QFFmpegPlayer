/********************************************************************************
** Form generated from reading UI file 'controlview.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLVIEW_H
#define UI_CONTROLVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlView
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_2;
    QSlider *horizontalSlider_progress;
    QHBoxLayout *horizontalLayout;
    QLabel *label_currentDuration;
    QLabel *label_2;
    QLabel *label_allDuration;
    QHBoxLayout *horizontalLayout_6;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButton_forwardSecond;
    QPushButton *pushButton_previous;
    QPushButton *pushButton_start;
    QPushButton *pushButton_next;
    QPushButton *pushButton_backwardSecond;
    QComboBox *comboBox_speed;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_soundOff;
    QSlider *horizontalSlider_volume;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButton_setting;
    QPushButton *pushButton_list;

    void setupUi(QWidget *ControlView)
    {
        if (ControlView->objectName().isEmpty())
            ControlView->setObjectName(QString::fromUtf8("ControlView"));
        ControlView->resize(905, 165);
        gridLayout = new QGridLayout(ControlView);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSlider_progress = new QSlider(ControlView);
        horizontalSlider_progress->setObjectName(QString::fromUtf8("horizontalSlider_progress"));
        horizontalSlider_progress->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(horizontalSlider_progress);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_currentDuration = new QLabel(ControlView);
        label_currentDuration->setObjectName(QString::fromUtf8("label_currentDuration"));

        horizontalLayout->addWidget(label_currentDuration);

        label_2 = new QLabel(ControlView);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        label_allDuration = new QLabel(ControlView);
        label_allDuration->setObjectName(QString::fromUtf8("label_allDuration"));

        horizontalLayout->addWidget(label_allDuration);

        horizontalLayout->setStretch(0, 2);
        horizontalLayout->setStretch(2, 2);

        horizontalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2->setStretch(0, 8);
        horizontalLayout_2->setStretch(1, 2);

        gridLayout->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pushButton_forwardSecond = new QPushButton(ControlView);
        pushButton_forwardSecond->setObjectName(QString::fromUtf8("pushButton_forwardSecond"));

        horizontalLayout_3->addWidget(pushButton_forwardSecond);

        pushButton_previous = new QPushButton(ControlView);
        pushButton_previous->setObjectName(QString::fromUtf8("pushButton_previous"));

        horizontalLayout_3->addWidget(pushButton_previous);

        pushButton_start = new QPushButton(ControlView);
        pushButton_start->setObjectName(QString::fromUtf8("pushButton_start"));

        horizontalLayout_3->addWidget(pushButton_start);

        pushButton_next = new QPushButton(ControlView);
        pushButton_next->setObjectName(QString::fromUtf8("pushButton_next"));

        horizontalLayout_3->addWidget(pushButton_next);

        pushButton_backwardSecond = new QPushButton(ControlView);
        pushButton_backwardSecond->setObjectName(QString::fromUtf8("pushButton_backwardSecond"));

        horizontalLayout_3->addWidget(pushButton_backwardSecond);


        horizontalLayout_6->addLayout(horizontalLayout_3);

        comboBox_speed = new QComboBox(ControlView);
        comboBox_speed->setObjectName(QString::fromUtf8("comboBox_speed"));

        horizontalLayout_6->addWidget(comboBox_speed);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        pushButton_soundOff = new QPushButton(ControlView);
        pushButton_soundOff->setObjectName(QString::fromUtf8("pushButton_soundOff"));

        horizontalLayout_4->addWidget(pushButton_soundOff);

        horizontalSlider_volume = new QSlider(ControlView);
        horizontalSlider_volume->setObjectName(QString::fromUtf8("horizontalSlider_volume"));
        horizontalSlider_volume->setOrientation(Qt::Horizontal);

        horizontalLayout_4->addWidget(horizontalSlider_volume);

        horizontalLayout_4->setStretch(0, 2);
        horizontalLayout_4->setStretch(1, 8);

        horizontalLayout_6->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        pushButton_setting = new QPushButton(ControlView);
        pushButton_setting->setObjectName(QString::fromUtf8("pushButton_setting"));

        horizontalLayout_5->addWidget(pushButton_setting);

        pushButton_list = new QPushButton(ControlView);
        pushButton_list->setObjectName(QString::fromUtf8("pushButton_list"));

        horizontalLayout_5->addWidget(pushButton_list);


        horizontalLayout_6->addLayout(horizontalLayout_5);

        horizontalLayout_6->setStretch(0, 6);
        horizontalLayout_6->setStretch(2, 2);
        horizontalLayout_6->setStretch(3, 2);

        gridLayout->addLayout(horizontalLayout_6, 1, 0, 1, 1);


        retranslateUi(ControlView);

        QMetaObject::connectSlotsByName(ControlView);
    } // setupUi

    void retranslateUi(QWidget *ControlView)
    {
        ControlView->setWindowTitle(QCoreApplication::translate("ControlView", "Form", nullptr));
        label_currentDuration->setText(QString());
        label_2->setText(QCoreApplication::translate("ControlView", "/", nullptr));
        label_allDuration->setText(QString());
        pushButton_forwardSecond->setText(QCoreApplication::translate("ControlView", "-10s", nullptr));
        pushButton_previous->setText(QCoreApplication::translate("ControlView", "pre", nullptr));
        pushButton_start->setText(QCoreApplication::translate("ControlView", "start", nullptr));
        pushButton_next->setText(QCoreApplication::translate("ControlView", "next", nullptr));
        pushButton_backwardSecond->setText(QCoreApplication::translate("ControlView", "+10s", nullptr));
        pushButton_soundOff->setText(QCoreApplication::translate("ControlView", "off", nullptr));
        pushButton_setting->setText(QCoreApplication::translate("ControlView", "setting", nullptr));
        pushButton_list->setText(QCoreApplication::translate("ControlView", "list", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ControlView: public Ui_ControlView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLVIEW_H
