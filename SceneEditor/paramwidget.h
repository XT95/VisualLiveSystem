#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H

#include <QDockWidget>

namespace Ui {
class ParamWidget;
}

class ParamWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit ParamWidget(QWidget *parent = 0);
    ~ParamWidget();

    void clearName();
public slots:
    void nameCCChanged(int id, QString name);

private slots:
    void CC0Changed(int);
    void CC1Changed(int);
    void CC2Changed(int);
    void CC3Changed(int);
private:
    Ui::ParamWidget *ui;
};

#endif // PARAMWIDGET_H
