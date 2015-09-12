#ifndef SCENE_H
#define SCENE_H

#include "texture.h"
#include "shader.h"
#include "fbo.h"
#include <QTime>
#include <QString>


typedef enum {DEFAULT, BLEND_ALPHA, BLEND_ADD, NONE} Mode;
typedef struct
{
    Shader *shader;
    FBO    *buffer, *backBuffer, *tmpBuffer;
    int     width, height;
    Mode    mode;
    QString name;
    Texture *channel[4];
    QString channelName[4];
}Layer;


//Structure décrivant une scene et permettant de les charger depuisun fichier
class Scene
{
public:
    Scene();
    ~Scene();

    void read(const char *filename = NULL); // Must be call where we are an OGL context
    void resetTime();
    void compute();
    void draw();

    void setChannel(int channel) { m_id = channel; }
    void setPreview(bool t);
    QString getParamName(int a) { return m_param[a]; }
private:
    Layer *m_layer;
    int m_nbLayer;
    QString m_param[4];

    QTime m_QTime;
    float m_time;
    int m_id;
};

#endif // SCENE_H
