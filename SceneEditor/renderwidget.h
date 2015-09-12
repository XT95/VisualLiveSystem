#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QTime>
#include <GL/glew.h>
#include <QGLWidget>
#include <QListWidget>
#include <scene.h>

#include "fbo.h"
#include "shader.h"

class RenderWidget : public QGLWidget
{
    Q_OBJECT

public:
       RenderWidget(QWidget *parent = 0);
       ~RenderWidget();

public slots:
       void resolutionChanged(int x,int y);
       void loadScene( QString filename );
       void recompilShader( QString txt, int id);


       void mousePressEvent(QMouseEvent* event);
       void mouseReleaseEvent(QMouseEvent* event);
       void mouseMoveEvent(QMouseEvent* event);
       void wheelEvent(QWheelEvent* event);

       void resetCamera();
       void takeScreenshot();
protected:
       void initializeGL();
       void paintGL();
       void resizeGL(int width, int height);

       QTime m_timer;
       QTime m_time;
       QTime m_fpsTime;
       int m_w,m_h;

       QPoint m_mousePos;
       bool m_mousePressed;
       float m_rayon;
       float m_theta, m_alpha;
};

#endif // RENDERWIDGET_H
