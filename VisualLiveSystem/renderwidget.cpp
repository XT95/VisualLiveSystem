#include <iostream>
#include <QMessageBox>
#include <QTimer>

#include "fast2dquad.h"
#include "renderwidget.h"
#include "core.h"


RenderWidget::RenderWidget(QWidget *parent) : QGLWidget(parent)
{

    //QGLFormat glFormat;
    //glFormat.setVersion( 3, 3 );
    //glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
    // this->setFormat(glFormat);


    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(1);

    m_postFX = NULL;
    m_finalPostFX = NULL;
    m_nbPostFX = 0;
    m_swap = 0;

    m_black = 0.;
    m_motionBlur = 0.1;
    m_gamma = 1.;
    m_contrast = 0.;
    m_desaturate = 0.;
    m_brightness = 0.;
    m_fisheye = 0.;

    m_time.restart();
    m_fpsTime.restart();
}

RenderWidget::~RenderWidget()
{
    Fast2DQuadFree();
    delete m_rtt[0];
    delete m_rtt[1];
    delete m_finalRtt;
    delete[] m_postFX;
    delete m_finalPostFX;
}

void RenderWidget::updateFinalPostFXData(float black, float motion, float c, float g, float d, float b, float f)
{
    m_black = black;
    m_motionBlur = motion;
    m_gamma = g;
    m_contrast = c;
    m_desaturate = d;
    m_brightness = b;
    m_fisheye = f;
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
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Core::instance()->initGL();
    m_finalRtt = new FBO();
    m_finalRtt->setSize(Core::instance()->getResX(), Core::instance()->getResY());
    Core::instance()->setFBOFinal(m_finalRtt);
    m_rtt[0] = new FBO();
    m_rtt[1] = new FBO();
    m_rtt[0]->setSize(Core::instance()->getResX(), Core::instance()->getResY());
    m_rtt[1]->setSize(Core::instance()->getResX(), Core::instance()->getResY());

    m_finalPostFX = new Shader();
    QFile file(":/res/shaders/postprocess.glsl");
    QString strings;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while (!in.atEnd()) {
            strings += in.readLine()+"\n";
        }
    }

    m_finalPostFX->compil(Core::instance()->getVertexShader(),strings.toStdString().c_str());


    {
        //Count ..
        int count = 0;
        QDir dir("./data/transitions");
        QStringList files = dir.entryList();
        for(int i=0; i<files.size(); i++)
        {
            QString item = files.at(i).toLocal8Bit().constData();

            if( item != "." && item != ".." && item != ".svn" )
            {
                count++;
            }
        }
        m_transition = new Transition[count];

        //And load !
        count = 0;
        files = dir.entryList();
        for(int i=0; i<files.size(); i++)
        {
            QString item = files.at(i).toLocal8Bit().constData();

            if( item != "." && item != ".." && item != ".svn" )
            {
                m_transition[count].read( item.toStdString().c_str() );
                count++;
            }
        }
    }
}


//-----------------------------------------------------------------------------
// paintGL
//-----------------------------------------------------------------------------
void RenderWidget::paintGL()
{
    updateTextureSpectrum();

    QTime time;
    time.restart();
    if(Core::instance()->getFBOChan(Core::instance()->getCurrentChannel()))
    {


            m_rtt[m_swap]->enable();
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    m_transition[Core::instance()->getCurrentTransition()].draw();
                glDisable(GL_BLEND);
            m_rtt[m_swap]->disable();

            for(int i=0; i<m_nbPostFX; i++)
            {
                m_swap = !m_swap;

                m_rtt[m_swap]->enable();
                    m_postFX[i].enable();
                    m_postFX[i].sendf("screen",Core::instance()->getResX(),Core::instance()->getResY());
                    m_postFX[i].sendf("bass",Core::instance()->getBass());
                    m_postFX[i].sendf("noise_lvl",Core::instance()->getNoiseLvl());
                        glBindTexture(GL_TEXTURE_2D, m_rtt[!m_swap]->getColor());
                        Fast2DQuadDraw();
                    m_postFX[i].disable();
                m_rtt[m_swap]->disable();
            }


        m_finalRtt->enable();
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            m_finalPostFX->enable();
                m_finalPostFX->sendf("black",m_black);
                m_finalPostFX->sendf("motionBlur",m_motionBlur);
                m_finalPostFX->sendf("gamma",m_gamma);
                m_finalPostFX->sendf("contrast",m_contrast);
                m_finalPostFX->sendf("desaturate",m_desaturate);
                m_finalPostFX->sendf("brightness",m_brightness);
                m_finalPostFX->sendf("fisheye",m_fisheye);
                glBindTexture(GL_TEXTURE_2D, m_rtt[m_swap]->getColor());
                Fast2DQuadDraw();
            m_finalPostFX->disable();
            glDisable(GL_BLEND);
        m_finalRtt->disable();

        glViewport(0,0,m_w,m_h);

        glBindTexture(GL_TEXTURE_2D, m_finalRtt->getColor());
        Fast2DQuadDraw();

        glFlush();
        glFinish();
    }
    Core::instance()->infoPostFX = time.elapsed();


    Core::instance()->infoFPS = 1000/(m_fpsTime.elapsed()+1);
    m_fpsTime.restart();
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

//-----------------------------------------------------------------------------
// loadPostEffects
//-----------------------------------------------------------------------------
void RenderWidget::loadPostEffects(QList<QListWidgetItem *> list)
{
    LoadingWidget* loading = Core::instance()->getLoadingWidget();
    loading->setTitle(QString("Post processing effects..."));
    loading->set(90);
    if(m_postFX)
    {
        delete[] m_postFX;
        m_postFX = NULL;
        m_nbPostFX = 0;
    }
    if(!list.isEmpty())
    {
        m_postFX = new Shader[list.size()];
        m_nbPostFX = list.size();
        for(int i=0; i<list.size(); i++)
        {
            QString path = QString("./data/postFX/")+list.at(i)->text();

            QFile file(path);
            QString strings;
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&file);
                while (!in.atEnd()) { strings += in.readLine()+"\n"; }
            }
            m_postFX[i].compil(Core::instance()->getVertexShader(),strings.toStdString().c_str());

            loading->set(i*10/list.size());
        }
    }
    loading->hide();
}


void RenderWidget::resolutionChanged(int x,int y)
{
    makeCurrent();
    m_rtt[0]->setSize(Core::instance()->getResX(), Core::instance()->getResY());
    m_rtt[1]->setSize(Core::instance()->getResX(), Core::instance()->getResY());
    m_finalRtt->setSize(Core::instance()->getResX(), Core::instance()->getResY());
}


void RenderWidget::updateTextureSpectrum()
{
    Core::instance()->fft->mutex.lock();
    glEnable(GL_TEXTURE_1D);
    Core::instance()->bindTextureSpectrum();
    glTexSubImage1D( GL_TEXTURE_1D, 0, 0, Core::instance()->fft->size()>>1, GL_LUMINANCE, GL_FLOAT,  Core::instance()->fft->getModule() );
    glDisable(GL_TEXTURE_1D);
    Core::instance()->fft->mutex.unlock();
}


void RenderWidget::transition()
{
    m_transition[Core::instance()->getCurrentTransition()].start();
}
