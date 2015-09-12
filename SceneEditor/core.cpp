#include <QTime>
#include <QString>
#include "core.h"
#include "scene.h"

Core* Core::m_instance = NULL;
Core* Core::instance()
{
    if(m_instance == NULL)
    {
        m_instance = new Core();
    }
    return m_instance;
}

Core::Core()
{
    m_resX = 640;
    m_resY = 360;

    m_scene = NULL;
}

static unsigned int mirand = 1;
float sfrand(void)
{
    float res;
    mirand *= 16807;
    *((unsigned int *) &res) = (mirand & 0x007fffff) | 0x40000000;
    return( res-3.0f );
}

void Core::initGL()
{
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_TEXTURE_2D);


    glEnable(GL_TEXTURE_3D);
    glGenTextures(1, &m_noise3D);
    glBindTexture(GL_TEXTURE_3D, m_noise3D);
    glTexParameteri(GL_TEXTURE_3D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    const int noiseSize=64;
    unsigned char *data = new unsigned char[noiseSize*noiseSize*noiseSize*4];
    for(int i=0;i<noiseSize*noiseSize*noiseSize*4;i++)
    {
        data[i] = (qrand()%256);
    }
    glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,noiseSize,noiseSize,noiseSize,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
    glBindTexture( GL_TEXTURE_3D, 0);
    glDisable(GL_TEXTURE_3D);
    delete[] data;
}



//Show
void Core::setCC(int id, float v)
{
    m_cc[id] = v;
}
float Core::getCC(int id)
{
    return m_cc[id];
}

//Rendering
void Core::setResolution(int x,int y)
{
    m_resX=x; m_resY=y;
    emit resolutionChanged(m_resX,m_resY);
}
int Core::getResX()
{
    return m_resX;
}
int Core::getResY()
{
    return m_resY;
}
int Core::getNoise3D()
{
    return m_noise3D;
}


//Utils
const char* Core::getVertexShader()
{

    static const char* vertexShader = "varying vec2 v;"
    ""
    "void main()"
    "{"
            "gl_Position    = gl_Vertex;"
            "v              = gl_Vertex.xy;"
            "gl_TexCoord[0] = gl_MultiTexCoord0;"
    "}";
    return vertexShader;
}

const char* Core::getVertexShaderParticle()
{

    static const char* vertexShaderParticle = "uniform sampler2D pos;"
    "void main()"
    "{"
            "gl_TexCoord[0] = gl_MultiTexCoord0;"

            "vec4 position = vec4(texture2D(pos, gl_MultiTexCoord0.xy).rgb,1.);"
            "vec4 eyePos = gl_ModelViewMatrix * position;"
            "vec4 projVoxel = gl_ProjectionMatrix * vec4(1.,1.,eyePos.z,eyePos.w);"
            "vec2 projSize = projVoxel.xy / projVoxel.w;"
            "gl_PointSize = 0.25 * (projSize.x+projSize.y);"
            //"gl_PointSize = min( pow(length(gl_ModelViewMatrixInverse[3]-position.xyz)-1.,2.), 1.)*20.+.1;"
            //"gl_PointSize = 20.;"
            "gl_Position = gl_ModelViewProjectionMatrix * position;"
    "}";
    return vertexShaderParticle;
}

QString Core::getDate()
{
    return QString("[")+QString::number(QTime::currentTime().hour())+":"+QString::number(QTime::currentTime().minute())+":"+QString::number(QTime::currentTime().second())+QString("] ");
}

//Log
void Core::writeInfo( QString txt, bool time)
{
    m_log->setTextColor( QColor(0,150,0) );
    if(time)
        txt = getDate() + txt;

    m_log->textCursor().movePosition(QTextCursor::End);
    m_log->insertPlainText(txt);
}
void Core::writeWarning( QString txt, bool time)
{
  //  m_log->clear();
    m_log->setTextColor( QColor(255,255,100) );
    if(time)
        txt = getDate() + txt;

    m_log->textCursor().movePosition(QTextCursor::End);
    m_log->insertPlainText(txt);
}
void Core::writeError( QString txt, bool time)
{
  //  m_log->clear();
    m_log->setTextColor( QColor(255,0,0) );
    if(time)
        txt = getDate() + txt;

    m_log->textCursor().movePosition(QTextCursor::End);
    m_log->insertPlainText(txt);
}
