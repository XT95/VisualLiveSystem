#include <QFile>
#include <QTextStream>
#include <QTimer>

#include "core.h"
#include "audiovisualizer.h"
#include "core.h"
#include "fast2dquad.h"


AudioVisualizer::AudioVisualizer(QGLWidget *share, QWidget *parent) : QGLWidget(parent,share)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(30);
}

AudioVisualizer::~AudioVisualizer()
{
    delete m_shader;
}


//-----------------------------------------------------------------------------
// initializeGL
//-----------------------------------------------------------------------------
void AudioVisualizer::initializeGL()
{
    QFile f(":res/shaders/audiovisualizer.glsl");
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    m_shader = new Shader();
    m_shader->compil(Core::instance()->getVertexShader(), ts.readAll().toStdString().c_str());
    m_shader->enable();
    m_shader->sendi("spectrum", 0 );
    m_shader->disable();
}


//-----------------------------------------------------------------------------
// paintGL
//-----------------------------------------------------------------------------
void AudioVisualizer::paintGL()
{
    m_shader->enable();
    m_shader->sendf("bassCutoff", (float)Core::instance()->bassCutoff  / (float) Core::instance()->pow2Size );
    Core::instance()->bindTextureSpectrum();
    Fast2DQuadDraw();
    m_shader->disable();

}

//-----------------------------------------------------------------------------
// resizeGL
//-----------------------------------------------------------------------------
void AudioVisualizer::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);
}

