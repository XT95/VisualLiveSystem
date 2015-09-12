#ifndef CHANNELWIDGET_H
#define CHANNELWIDGET_H

#include <GL/glew.h>
#include <QGLWidget>
#include <QDockWidget>
#include <QListWidget>
#include "scene.h"


class GLScene;

namespace Ui {
class ChannelWidget;
}

class ChannelWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit ChannelWidget(int id, QGLWidget *share = 0, QWidget *parent = 0);
    ~ChannelWidget();
public slots:
    void updateList( QList<QListWidgetItem *> list );
    void selectChanged(int);
    void CCChanged(int, float);
    void CCChanged();
    void sceneUp();
    void sceneDown();
    void Switch();
private:
    Ui::ChannelWidget *ui;
    int m_id;
    GLScene *m_glscene;
    Scene *m_scene;
};

#endif // CHANNELWIDGET_H
