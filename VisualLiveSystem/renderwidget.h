#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QTime>
#include <GL/glew.h>
#include <QGLWidget>
#include <QListWidget>
#include <scene.h>

#include "soundmanager.h"
#include "fbo.h"
#include "shader.h"
#include "transition.h"

class RenderWidget : public QGLWidget
{
    Q_OBJECT

public:
       RenderWidget(QWidget *parent = 0);
       ~RenderWidget();

public slots:
       void loadPostEffects( QList<QListWidgetItem *> list );
       void resolutionChanged(int x,int y);
       //void updateTextureSpectrumChannel2(fftw_complex *out);
       void updateFinalPostFXData(float black, float motion, float c, float g, float d, float b, float f);
       void transition();

protected:
       void updateTextureSpectrum();
       virtual void initializeGL();
       virtual void paintGL();
       virtual void resizeGL(int width, int height);

       QTime m_timer;
       QTime m_time;
       QTime m_fpsTime;
       int m_w,m_h;

       Shader *m_postFX,*m_finalPostFX;
       int m_nbPostFX;
       FBO *m_rtt[2],*m_finalRtt;
       bool m_swap;

       float m_black,m_motionBlur,m_contrast,m_gamma,m_desaturate,m_brightness, m_fisheye;

       Transition *m_transition;
};

#endif // RENDERWIDGET_H
