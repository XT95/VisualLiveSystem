#ifndef DATA_H
#define DATA_H
#include <QtCore>
#include <QObject>
#include <GL/glew.h>
#include <QGLWidget>
#include <QTextEdit>

class Scene;
class FBO;



#define BUFFER_OFFSET(a) ((char*)NULL + (a))
struct vec2
{
    float x,y;
};


class Core : public QObject
{
    Q_OBJECT
public:
    //Singleton !
    static Core* instance();
    void initGL();

    //Show !
    void setCC(int id, float v);
    float getCC(int id);

    //Log
    void setLogWidget( QTextEdit *w ) { m_log = w; }
    void writeInfo( QString txt, bool time=true);
    void writeWarning( QString txt, bool time=true );
    void writeError( QString txt, bool time=true );

    //Rendering
    Scene* getScene() { return m_scene; }
    void setScene( Scene *sc ) { m_scene = sc; }
    void setResolution(int x,int y);
    int getResX();
    int getResY();
    int getNoise3D();


    //Utils
    Scene* getDefaultScene();
    const char* getVertexShader();
    const char* getVertexShaderParticle();
    void setEffectName( QString name ) { m_effectName = name; }
    QString getEffectName() { return m_effectName; }
    QString getDate();

signals:
    void resolutionChanged(int,int);


private:
    Core();
    Scene *m_scene;
    static Core *m_instance;
    int m_resX,m_resY;
    GLuint m_noise3D;
    float m_cc[4];
    QTextEdit *m_log;
    QString m_effectName;

};

#endif // DATA_H
