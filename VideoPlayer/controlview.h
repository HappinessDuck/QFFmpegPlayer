#ifndef CONTROLVIEW_H
#define CONTROLVIEW_H
#include <QWidget>

namespace Ui {
class ControlView;
}

class ControlView : public QWidget
{
    Q_OBJECT

public:
    explicit ControlView(QWidget *parent = nullptr);
    ~ControlView();

private:
    Ui::ControlView *ui;





};

#endif // CONTROLVIEW_H
