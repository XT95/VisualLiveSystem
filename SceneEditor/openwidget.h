#ifndef OPENWIDGET_H
#define OPENWIDGET_H

#include <QWidget>

namespace Ui {
class OpenWidget;
}

class OpenWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit OpenWidget(QWidget *parent = 0);
    ~OpenWidget();
    
public slots:
    void ok();
    void cancel();

signals:
    void open(QString);
private slots:
    void refreshList();
private:
    Ui::OpenWidget *ui;
};

#endif // OPENWIDGET_H
