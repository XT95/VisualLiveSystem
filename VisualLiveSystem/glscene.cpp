#include <iostream>
#include <QTimer>
#include "core.h"
#include "fast2dquad.h"
#include "glscene.h"


GLScene::GLScene(QGLWidget *share, int channel, QWidget *parent, bool chanID) :
    QGLWidget(parent,share),
    m_id(channel),
    m_chanID(chanID)
{
    m_scene = NULL;
    m_selectID = 0;
    m_nbScene = 0;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(1);

    connect(Core::instance(), SIGNAL(resolutionChanged(int,int)), this, SLOT(resolutionChanged(int,int)));
}



//-----------------------------------------------------------------------------
// compilScenes
//-----------------------------------------------------------------------------
void GLScene::compilScenes( Scene *scene, QList<QListWidgetItem *> list )
{
    makeCurrent();
    m_scene = scene;
    m_nbScene = list.size();
    LoadingWidget* loading = Core::instance()->getLoadingWidget();
    loading->setTitle(QString("Scenes..."));
    if(m_id == 0)
    {
        loading->set(0);
    }
    else
        loading->set(45);

    for(int i=0; i<list.size(); i++)
    {
        if(m_id == 0)
            loading->set(i*45/list.size());
        else
            loading->set(45+i*45/list.size());
        m_scene[i].setChannel(m_id);
        m_scene[i].read(list.at(i)->text().toStdString().c_str());
    }
    loading->hide();
}
void GLScene::setCurrent(int a)
{
    m_selectID = a;
}

//-----------------------------------------------------------------------------
// initializeGL
//-----------------------------------------------------------------------------
void GLScene::initializeGL()
{
    m_rtt = new FBO();
    Core::instance()->setFBOChan(m_rtt,m_id);
    if(m_id == Core::instance()->getCurrentChannel())
    {
        m_rtt->setSize(Core::instance()->getResX(), Core::instance()->getResY());
    }
    else
    {
        m_rtt->setSize(Core::instance()->getResX()/6, Core::instance()->getResY()/6);
    }
}


//-----------------------------------------------------------------------------
// paintGL
//-----------------------------------------------------------------------------
void GLScene::paintGL()
{
    QTime time;
    time.restart();

    if(m_scene)
    {
        m_scene[m_selectID].compute();
        m_rtt->enable();
                m_scene[m_selectID].draw();
        m_rtt->disable();
    }
    else
    {
        Core::instance()->getDefaultScene()->compute();
        m_rtt->enable();
            Core::instance()->getDefaultScene()->draw();
        m_rtt->disable();
    }
    glViewport(0,0,m_w,m_h);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_rtt->getColor());
    Fast2DQuadDraw();

    glFlush();
    glFinish();

    if(m_chanID==0)
        Core::instance()->infoChannelA = time.elapsed();
    else
        Core::instance()->infoChannelB = time.elapsed();
}

//-----------------------------------------------------------------------------
// resizeGL
//-----------------------------------------------------------------------------
void GLScene::resizeGL(int width, int height)
{
    m_w = width;
    m_h = height;
}



void GLScene::resolutionChanged(int x,int y)
{
    makeCurrent();
    if(m_id == Core::instance()->getCurrentChannel())
    {
        m_rtt->setSize(Core::instance()->getResX(), Core::instance()->getResY());
        for(int i=0; i<m_nbScene; i++)
            m_scene[i].setPreview(false);
    }
    else
    {
        m_rtt->setSize(Core::instance()->getResX()/8, Core::instance()->getResY()/8);
        for(int i=0; i<m_nbScene; i++)
            m_scene[i].setPreview(true);
    }
}
