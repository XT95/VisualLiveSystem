#ifndef TRANSITION_H
#define TRANSITION_H

#include "texture.h"
#include "shader.h"
#include "fbo.h"
#include <QTime>
#include <QString>

class Transition
{
public:
    Transition();
    ~Transition();

    void read(const char *filename = NULL); // Must be call where we are an OGL context
    void start();
    void draw();

private:
    Shader *m_shader;
    FBO *m_backBuffer;
    //Texture m_texture[4];
    float m_totalTime,m_switchTime;
    QTime m_time;
    bool m_switched;
};


#endif // TRANSITION_H
