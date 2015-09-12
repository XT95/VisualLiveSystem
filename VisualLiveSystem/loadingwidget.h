#ifndef LOADINGWIDGET_H
#define LOADINGWIDGET_H

#include <QWidget>

namespace Ui {
class LoadingWidget;
}

class LoadingWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit LoadingWidget(QWidget *parent = 0);
    ~LoadingWidget();
    
    void setTitle(QString t);
    void set(int a);
private:
    Ui::LoadingWidget *ui;
};

#endif // LOADINGWIDGET_H
