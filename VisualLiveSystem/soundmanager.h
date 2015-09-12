#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <math.h>
#include <vector>
#include <signal/signal.hpp>

#include <QWidget>
#include <QObject>
#include <QTime>
#include <QThread>

#include <RTAudio.h>
#include "FFTAnalyzer.h"

#include "signal/audiopipe.hpp"
#include <QList>

#include <QListWidget>

namespace Ui {
class SoundManager;
}

class PlaylistWidget : public QListWidget
{
    Q_OBJECT
public:
    PlaylistWidget(QWidget* parent=0);

protected:
    virtual bool dropMimeData(int index,const QMimeData* data, Qt::DropAction action );
    virtual QStringList mimeTypes() const;


};


class SoundManager : public QWidget
{
        Q_OBJECT
public:
    explicit SoundManager(AudioPipe& pipe, QWidget *parent = 0);
    ~SoundManager();

    void setAudioPipe(AudioPipe& pipe);
    void refreshApis();

public slots:
    void startRecord();
    void apiChanged(QString api);
    void ackApply();
    void receivedLatency(unsigned int);
    void pow2SizeChanged();

private slots:
    void cancel();
    void apply();
    void ok();
    void refreshDeviceList();
    void enableMixer(bool e);
    void nextTrack();
    void playTrack();

    void audioThreadRunning();
    void audioThreadStopped();

private:    
    AudioPipe* _pipe;
    Ui::SoundManager *ui;
    QList<unsigned int> _inputTable;
    QList<unsigned int> _outputTable;

};

#endif // SOUNDMANAGER_H
