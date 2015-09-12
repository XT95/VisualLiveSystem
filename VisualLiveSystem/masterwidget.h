#ifndef MASTERWIDGET_H
#define MASTERWIDGET_H

#include <QDockWidget>
#include <QTimer>
#include <QTime>
#include "audiovisualizer.h"

namespace Ui {
class MasterWidget;
}

class MasterWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit MasterWidget(QGLWidget *share, QWidget *parent = 0);
    ~MasterWidget();
signals:
    void finalPostFXDataChanged(float,float,float,float,float,float,float);
    void transition();

public slots:
    void inLive(bool t);
    void gammaChanged(int v);
    void brightnessChanged(int v);
    void contrastChanged(int v);
    void desaturateChanged(int v);
    void blackFadeChanged(int v);
    void timeSpeedChanged(int v);
    void bassTimeSpeedChanged(int v);
    void transitionUp();
    void transitionDown();
    void Switch();
    void updateInfo();

private slots:
    void postProcessingChanged();
    void sliderChanged();
    void transitionChanged(int n);
    void basscutOffChanged(int i);
    void pow2Changed(unsigned int p);

private:
    Ui::MasterWidget *ui;
    AudioVisualizer *m_audiovisu;
    QTime m_timer;
};

#endif // MASTERWIDGET_H
