#include <iostream>
#include <QMessageBox>
#include <QTimer>
#include <QFileDialog>
#include <QDir>
#include <QMouseEvent>

#include "fast2dquad.h"
#include "renderwidget.h"
#include "core.h"


RenderWidget::RenderWidget(QWidget *parent) : QGLWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(10);

    resetCamera();

    m_time.restart();
    m_fpsTime.restart();
}

RenderWidget::~RenderWidget()
{
    Fast2DQuadFree();
}


//-----------------------------------------------------------------------------
// initializeGL
//-----------------------------------------------------------------------------
void RenderWidget::initializeGL()
{
    //Glew
    GLenum err = glewInit();
    if(GLEW_OK != err)
    {
        QMessageBox::critical(this, "Error", "GLEW Init fail !");
    }
    if(!GLEW_ARB_vertex_program || !GLEW_ARB_fragment_program)
    {
        QMessageBox::critical(this, "Error", "GLSL Shader not supported !");
    }

    Fast2DQuadInit();
    glMatrixMode(GL_PROJECTION);
    gluPerspective(90., 16./9., 0.01, 10000.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    Core::instance()->initGL();

}

//-----------------------------------------------------------------------------
// loadScene
//-----------------------------------------------------------------------------
void RenderWidget::loadScene(QString filename)
{

    Scene *oldS = NULL;
    Scene *newS = new Scene();

    newS->read(filename.toStdString().c_str());
    if( Core::instance()->getScene() )
    {
        oldS = Core::instance()->getScene();
    }

    Core::instance()->setScene(newS);
    delete oldS;
}

//-----------------------------------------------------------------------------
// recompilShader
//-----------------------------------------------------------------------------
void RenderWidget::recompilShader(QString txt,int id)
{
    Core::instance()->getScene()->compilShader(txt.toStdString().c_str(),id);
}

//-----------------------------------------------------------------------------
// paintGL
//-----------------------------------------------------------------------------
void RenderWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if( Core::instance()->getScene() )
    {
        if(m_alpha < 0.005f) m_alpha = 0.005f;
        if(m_alpha > 3.14f)  m_alpha = 3.14f;
        float px = sin(m_theta)*m_rayon*sin(m_alpha);
        float py = cos(m_alpha)*m_rayon;
        float pz = cos(m_theta)*m_rayon*sin(m_alpha);

        glLoadIdentity();
        gluLookAt( px,py,pz, 0.,0.,0., 0.,1.,0.);
        Core::instance()->getScene()->compute();

        Core::instance()->getScene()->draw();
    }
    glFlush();
    glFinish();
}

//-----------------------------------------------------------------------------
// resizeGL
//-----------------------------------------------------------------------------
void RenderWidget::resizeGL(int width, int height)
{
    m_w = width;
    m_h = height;
    glViewport(0,0,m_w,m_h);
}


void RenderWidget::resolutionChanged(int x,int y)
{
    makeCurrent();
}



void RenderWidget::mousePressEvent(QMouseEvent* event)
{
    m_mousePos = event->pos();
    if(event->button() == Qt::LeftButton)
        m_mousePressed = true;
    QWidget::mousePressEvent(event);
}

void RenderWidget::mouseReleaseEvent(QMouseEvent* event)
{
    m_mousePos = event->pos();
    if(event->button() == Qt::LeftButton)
        m_mousePressed = false;
    QWidget::mouseReleaseEvent(event);
}

void RenderWidget::mouseMoveEvent(QMouseEvent* event)
{
    if(m_mousePressed)
    {
        m_theta -= 0.004f * float(event->pos().x() - m_mousePos.x());
        m_alpha -= 0.004f * float(event->pos().y() - m_mousePos.y());
    }
    m_mousePos = event->pos();

    QWidget::mouseMoveEvent(event);
}

void RenderWidget::wheelEvent(QWheelEvent* event)
{

    if(event->delta() > 0) m_rayon /= 1.05f;
      else m_rayon *= 1.05f;

    QWidget::wheelEvent(event);
}

void RenderWidget::resetCamera()
{
    m_rayon = 10.;
    m_theta = 0.f;
    m_alpha = 0.f;
    m_mousePressed = false;
}


void RenderWidget::takeScreenshot()
{
    emit updateGL();
    QImage imageToSave = grabFrameBuffer(false);
    QString filename = QFileDialog::getSaveFileName(this, "Save a screenshot", QString(), "Images (*.png *.jpg)");
    imageToSave.save(filename);
}
