#ifndef PROJECTORWIDGET_H
#define PROJECTORWIDGET_H


#include <QTime>
#include <GL/glew.h>
#include <QGLWidget>

#include "fbo.h"

class ProjectorWidget : public QGLWidget
{
    Q_OBJECT
public:
    ProjectorWidget(QGLWidget *share);
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mouseDoubleClickEvent(QMouseEvent *event);
private:
    QTime m_timer;
    int m_w,m_h;
    bool m_fullscreen;
};

#endif // PROJECTORWIDGET_H
