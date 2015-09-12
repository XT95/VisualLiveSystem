#ifndef SCENE_H
#define SCENE_H

#include "texture.h"
#include "shader.h"
#include "fbo.h"
#include <QTime>
#include <QString>

typedef enum {DEFAULT, BLEND_ALPHA, BLEND_ADD, NONE} Mode;
typedef enum {PIXEL,PARTICLE} Type;
typedef struct
{
    Shader *shader;
    FBO    *buffer, *backBuffer, *tmpBuffer;
    int     width, height;
    Type    type;
    Mode    mode;
    QString name;
    Texture *channel[4];
    QString channelName[4];
    int     iterate;
}Layer;
typedef struct
{
    FBO *src,*dest;
}Copy;

class Scene
{
public:
    Scene();
    ~Scene();

    void compilShader(const char *txt, int id);
    void read(const char *filename = NULL); // Must be call where we are an OGL context


    void resetTime();
    void compute();
    void draw();

    QString getParamName(int a) { return m_param[a]; }
    Layer getLayer(int id) { return m_layer[id]; }
private:
    void generateParticles();
    void drawParticles();

    int m_nbParticles;
    GLuint m_vboParticle;

    Layer *m_layer;
    int m_nbLayer;
    Copy *m_copy;
    int m_nbCopy;
    QString m_param[4];

    QTime m_QTime;
    float m_time;
};

#endif // SCENE_H
