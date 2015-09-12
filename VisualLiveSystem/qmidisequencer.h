#ifndef QMIDISEQUENCER_H
#define QMIDISEQUENCER_H


#include <QObject>
#include <QThread>
#include <portmidi.h>

class QMidiSequencer : public QThread
{
Q_OBJECT
public:
    QMidiSequencer(QString clientName, int device, QObject* parent = 0);
    ~QMidiSequencer();
private:
    PmStream *m_midi;
    PmEvent m_buffer[256];
    void run();

signals:
    void clientConnected(int clientId, int clientPort);
    void clientParted(int clientId, int clientPort);

    void controllerValueChanged(int ccId, int newValue);

    void noteOn(int channel, int note, int velocity);
    void noteOff(int channel, int note);

public slots:

};

#endif // QMIDISEQUENCER_H
