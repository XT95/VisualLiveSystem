#include <QTimer>
#include <QIcon>

#include "core.h"
#include "fast2dquad.h"
#include "projectorwidget.h"

ProjectorWidget::ProjectorWidget(QGLWidget *share) :
    QGLWidget(NULL, share)
{
    m_fullscreen = false;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(1);

    this->setWindowTitle("Projector");
    this->setWindowIcon( QIcon(QString(":/res/img/projector.png")) );
}

//-----------------------------------------------------------------------------
// initializeGL
//-----------------------------------------------------------------------------
void ProjectorWidget::initializeGL()
{

}

void ProjectorWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(m_fullscreen)
    {
        this->setCursor( QCursor(Qt::ArrowCursor) );
        showNormal();
    }
    else
    {
        this->setCursor( QCursor(Qt::BlankCursor) );
        showFullScreen();
    }
    m_fullscreen=!m_fullscreen;
}

//-----------------------------------------------------------------------------
// paintGL
//-----------------------------------------------------------------------------
void ProjectorWidget::paintGL()
{
    if(Core::instance()->getFBOChan(Core::instance()->getCurrentChannel()))
    {
        glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, Core::instance()->getFBOFinal()->getColor());
            Fast2DQuadDraw();
        glFinish();
    }
}

//-----------------------------------------------------------------------------
// resizeGL
//-----------------------------------------------------------------------------
void ProjectorWidget::resizeGL(int width, int height)
{
    m_w = width;
    m_h = height;
    glViewport(0,0,m_w,m_h);
}
