#include "core.h"
#include "scene.h"
#include "soundmanager.h"

Core* Core::m_instance = NULL;
Core* Core::instance()
{
    if(m_instance == NULL)
    {
        m_instance = new Core();
    }
    return m_instance;
}

Core::Core() :
    settings("settings.ini",QSettings::IniFormat)
{
    m_currentChannel = 0;
    m_resX = 1280;
    m_resY = 720;
    m_fboChan[0] = NULL;
    m_fboChan[1] = NULL;
    m_fboFinal = NULL;

    m_bass = 0.f;
    m_bassTime = 0.f;
    m_timeSpeed = .4f;
    m_bassTimeSpeed = .4f;
    m_noiseLvl=0.f;
    m_bassMotion = 0.0f;
    m_transition = 0;

    m_inLive = true;

    infoChannelA = 0;
    infoChannelB = 0;
    infoPostFX = 0;
    infoFPS = 0;
    infoSound = 0;


    for(int i=0; i<2; i++)
        for(int j=0; j<4; j++)
            m_cc[i][j]=0.f;
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
    glEnable(GL_TEXTURE_1D);
    glEnable(GL_TEXTURE_2D);

    m_defaultScene = new Scene();
    m_defaultScene->read(NULL);

    glGenTextures(1, &m_spectrum);
    glBindTexture( GL_TEXTURE_1D, m_spectrum);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexImage1D( GL_TEXTURE_1D, 0, GL_LUMINANCE, pow2Size>>1, 0, GL_LUMINANCE, GL_FLOAT, NULL );


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

void Core::initAudio()
{
    pow2Size = settings.value("Signal/Pow2Size",1024).toUInt();
    if (pow2Size != 256 && pow2Size != 1024 && pow2Size != 4096) {
        pow2Size = 1024;
        settings.setValue("Signal/Pow2Size",1024);
    }

   if (settings.value("Signal/UseFramerateMethod",true).toBool() && pow2Size >= 1024)
       Signal::globalConfigurationFromFramerate(settings.value("Signal/Frequency",44100).toUInt(),
                                                settings.value("Signal/RefreshRate",50.f).toFloat());
   else
       Signal::globalConfigurationFromPow2(settings.value("Signal/Frequency",44100).toUInt(),pow2Size);

   audioPipe = new AudioPipe;
   decoder = new BassDecoder;
   mixer = new PlaylistMixer(*decoder);
   fft = new FFT(pow2Size);
   visualSignal=new VisualSignal;

   bassCutoff = 7;

   connect(audioPipe, SIGNAL(inLive(bool)), this, SLOT(liveChanged(bool)));
}

void Core::setPow2Size(unsigned int s)
{
    pow2Size=s;
    if (pow2Size != 256 && pow2Size != 1024 && pow2Size != 4096) {
        pow2Size = 1024;
    }
    settings.setValue("Signal/Pow2Size",pow2Size);
    if (Signal::isPow2)
        Signal::globalConfigurationFromPow2(settings.value("Signal/Frequency",44100).toUInt(),pow2Size);

    fft->mutex.lock();
    fft->realloc(pow2Size);
    fft->mutex.unlock();

    glEnable(GL_TEXTURE_1D);
    glBindTexture( GL_TEXTURE_1D, m_spectrum);
    glTexImage1D( GL_TEXTURE_1D, 0, GL_LUMINANCE, pow2Size>>1, 0, GL_LUMINANCE, GL_FLOAT, NULL );

    emit pow2Changed(pow2Size);
}

void Core::initWidgets()
{
   m_loadingw = new LoadingWidget();
}

//Channel
void Core::Switch()
{
    m_currentChannel = !m_currentChannel;
    m_bassTime=0.;
    emit resolutionChanged(m_resX,m_resY);
    emit channelSwitched();
}
int Core::getCurrentChannel()
{
    return m_currentChannel;
}

float Core::getNoiseLvl(){
    return m_noiseLvl;
}

void Core::setNoise(float n)
{
    m_noiseLvl=n;
}

//Transition
void Core::setCurrentTransition(int id)
{
    m_transition = id;
}
int Core::getCurrentTransition()
{
    return m_transition;
}


//Show
void Core::setCC(int c, int id, float v)
{
    m_cc[c][id] = v;
}
float Core::getCC(int c, int id)
{
    return m_cc[c][id];
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


//Spectrum & Time
void Core::bindTextureSpectrum()
{
    glBindTexture( GL_TEXTURE_1D, m_spectrum );
}
void Core::setBass(float bass)
{
    //m_b = m_b*m_bassMotion + bass*(1.-m_bassMotion); //si je l'enleve c'est uniquement parce que je ne sait pas Ã  quoi sa sert...
    const float signal_f=Signal::refreshRate;
    const float signal_t=1/signal_f;
    m_bass = bass*(1.f-m_bassMotion) + m_bass*m_bassMotion; //Thread-Safe
    const float temp = m_bass*m_bassTimeSpeed*20.*signal_t + m_bassTime; // en "bass*sec" 1 bass*sec = "boum Ã  120bpm"
    m_bassTime = temp; //Thread-Safe
    //m_bassTime += m_b*m_bassTimeSpeed*.25; //Non Thread-Safe
    //qDebug() << "bass " << bass << " Bass Time " << m_bassTime;
}
void Core::setTimeSpeed(float t)
{
    m_timeSpeed = t;
}
void Core::setBassTimeSpeed(float t)
{
    m_bassTimeSpeed = t;
}

void Core::resetBassTime()
{
    m_bassTime = 0;
}

float Core::getTimeSpeed()
{
    return m_timeSpeed;
}

float Core::getBass()
{
    return m_bass;
}

float Core::getBassTime()
{
    return m_bassTime;
}
float Core::getBassTimeSpeed()
{
    return m_bassTimeSpeed;
}


//FBO
void Core::setFBOChan(FBO *fbo, int n)
{
    m_fboChan[n] = fbo;
}
FBO *Core::getFBOChan(int n)
{
    return m_fboChan[n];
}
void Core::setFBOFinal(FBO *fbo)
{
    m_fboFinal = fbo;
}
FBO* Core::getFBOFinal()
{
    return m_fboFinal;
}

//Utils
Scene* Core::getDefaultScene()
{
    return m_defaultScene;
}

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

LoadingWidget* Core::getLoadingWidget()
{
    return m_loadingw;
}

//Show !
void Core::liveChanged(bool t)
{
    m_inLive = true;//t;
}
