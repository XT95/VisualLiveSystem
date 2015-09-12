#ifndef GLSCENE_H
#define GLSCENE_H

#include <QLabel>
#include <QTime>
#include <GL/glew.h>
#include <QGLWidget>
#include <QListWidget>
#include "scene.h"
#include "fbo.h"



//Mini widget de prévisu situé dans les channels widgets
class GLScene : public QGLWidget
{
    Q_OBJECT
public:
    GLScene(QGLWidget *share, int channel, QWidget *parent, bool chanID);

    void compilScenes( Scene *scene, QList<QListWidgetItem *> list );
    void setCurrent( int a );

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

private slots:
    void resolutionChanged(int,int);
private:
    QTime m_timer;
    int m_w,m_h, m_id;

    FBO *m_rtt;
    Scene *m_scene;
    int m_nbScene;
    int m_selectID;
    bool m_chanID;
};

#endif // GLSCENE_H
