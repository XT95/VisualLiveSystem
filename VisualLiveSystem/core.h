#ifndef DATA_H
#define DATA_H
#include <QtCore>
#include <QObject>
#include <GL/glew.h>
#include <QGLWidget>
#include <QSettings>

#include "loadingwidget.h"
#include "signal/audiopipe.hpp"
#include "signal/decoder.hpp"
#include "signal/fft.hpp"

class Scene;
class FBO;

class Core : public QObject
{
    Q_OBJECT
public:
    //Singleton !
    static Core* instance();
    void initGL();
    void initAudio();
    void initWidgets();

    //Show !
    bool inLive() { return m_inLive; }
    void setBassMotion(float b) { m_bassMotion=b; }
    void setCC(int c, int id, float v);
    float getCC(int c, int id);

    //Transition
    void setCurrentTransition(int id);
    int getCurrentTransition();

    //Channel
    void Switch();
    int getCurrentChannel();

    //Rendering
    void setResolution(int x,int y);
    int getResX();
    int getResY();
    int getNoise3D();

    //FBO
    void setFBOChan(FBO *fbo, int n);
    FBO* getFBOChan(int n);
    void setFBOFinal(FBO *fbo);
    FBO* getFBOFinal();

    //Spectrum & Time
    void bindTextureSpectrum();
    void setBass(float bass);
    void setTimeSpeed(float t);
    void setBassTimeSpeed(float t);
    void resetBassTime();
    void setNoise(float n);
    float getTimeSpeed();
    float getBassTimeSpeed();
    float getBass();
    float getBassTime();
    float getNoiseLvl();


    //Utils
    Scene* getDefaultScene();
    const char* getVertexShader();
    LoadingWidget* getLoadingWidget();

    //Audio
    unsigned int bassCutoff;
    AudioPipe* audioPipe;
    Decoder* decoder;
    PlaylistMixer* mixer;
    FFT* fft;
    VisualSignal* visualSignal;
    unsigned int pow2Size;


    void setPow2Size(unsigned int s);


    //Infos
    int infoChannelA,infoChannelB,infoPostFX,infoFPS,infoSound;

    //Settings
    QSettings settings;


signals:
    void resolutionChanged(int,int);
    void channelSwitched();
    void pow2Changed(unsigned int);

public slots:
    void liveChanged(bool);

private:
    Core();

    static Core *m_instance;
    Scene *m_defaultScene;
    int m_currentChannel;
    int m_resX,m_resY;
    FBO *m_fboChan[2];
    FBO *m_fboFinal;
    GLuint m_spectrum;
    GLuint m_noise3D;

    LoadingWidget *m_loadingw;
    float m_bass,m_bassTime, m_timeSpeed, m_bassTimeSpeed, m_bassMotion, m_noiseLvl;
    bool m_inLive;
    float m_cc[2][4];
    int m_transition;

};

#endif // DATA_H
