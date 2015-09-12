#ifndef BOTTOMWIDGET_H
#define BOTTOMWIDGET_H

#include <QDockWidget>
#include <QTextEdit>

namespace Ui {
class BottomWidget;
}

class BottomWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit BottomWidget(QWidget *parent = 0);
    ~BottomWidget();
    
    QTextEdit* getLogWidget();

private slots:
    void logChanged();
private:
    Ui::BottomWidget *ui;
};

#endif // BOTTOMWIDGET_H
