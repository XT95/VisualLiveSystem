#ifndef AUDIOVISUALIZER_H
#define AUDIOVISUALIZER_H

#include "shader.h"
#include <QTime>
#include <QString>
#include <QGLWidget>

class AudioVisualizer : public QGLWidget
{
public:
    AudioVisualizer(QGLWidget *share, QWidget *parent);
    ~AudioVisualizer();

protected:
       void initializeGL();
       void paintGL();
       void resizeGL(int width, int height);

private:
    Shader *m_shader;
};


#endif // AUDIOVISUALIZER_H
