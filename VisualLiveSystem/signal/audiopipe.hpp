/****************************************************************************
Nom ......... : audiopipe.hpp
Role ........ : Déclare une classe gérant le thread de calcul du son et les
                objets Signals
Auteur ...... : Julien DE LOOR
Version ..... : V1.0 olol
Licence ..... : © Copydown™
****************************************************************************/

#ifndef AUDIOPIPE_HPP
#define AUDIOPIPE_HPP

#include <QThread>
#include <RtAudio.h>
#include <QMutex>
#include <QWaitCondition>
#include <QSemaphore>
#include <vector>

#include "signal/signal.hpp"

QString RtAudioApiToQString(RtAudio::Api api);
RtAudio::Api QStringToRtAudioApi(const QString& api);

class AudioPipe : public QThread
{
    Q_OBJECT
public:
    explicit AudioPipe(QObject *parent = 0);
    void setApi(RtAudio::Api api); //Ceci reset entièrement RtAudio avec l'API voulue renvoie
                                   //faux si c'est un echec, il n'y alors aucune API de chargée
                                   //L'AudioPipe passe en STOP !

    bool setInputDeviceId(int id); //Change for this device next restart
    bool setOutputDeviceId(int id); //Change for this device
    void setMixer(AbstractStereoSignalMixer* mixer); //Mixer input/output


    //Relais RtAudio (inutile de passer en stop)
    inline RtAudio::Api getCurrentApi() { return _driver->getCurrentApi(); }
    inline unsigned int getDeviceCount()  { return _driver->getDeviceCount(); }
    inline RtAudio::DeviceInfo getDeviceInfo(unsigned int device) { return _driver->getDeviceInfo(device); }

    inline QMutex& getMixerMutex() {return _mutexMixer;}



signals:
    void inLive(bool); //Vrai si le driver est actif, faux sinon
    void critical_error(QString title, QString error);
    void computedLatency(unsigned int);

public slots:
    void stop(); //Stop RtAudio et le thread de calcul, DOIT être appelé avant de modifier quelque chose
                 //sur la configuration de l'OBJET
    void prepare();

    void setLatenyTime(float sec);



protected:
    //Thread de calcul (peut être lent)
    virtual void run();

private slots:
    void show_critical(QString caption, QString error);

private:
    inline long getStreamLatency() { return _driver->getStreamLatency(); }

    std::vector<Signal> _latencySignalsR;
    std::vector<Signal> _latencySignalsL;
    int _latencyTimeBuffer;
    int _latencyCurrentBuffer;

    //Callback RtAudio (doit être le plus rapide possible)
    static int rtaudio_callback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data );

    int _deviceOutId; //-1 = pas d'output
    int _deviceInId;  //-1 = pas d'input
    RtAudio* _driver; //Driver Audio en dehors d'un appel à "setApi" _driver est toujours alloué
    AbstractStereoSignalMixer* _mixer;

    volatile bool _stopThread; //Signale au thread qu'il doit s'arreter

    Signal _leftOut;
    Signal _rightOut;
    Signal _leftIn;
    Signal _rightIn;

    QMutex _mutexIO;
    QMutex _mutexMixer;
    QWaitCondition _contidionIO;

};

#endif // AUDIOPIPE_HPP
