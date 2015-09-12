#ifndef MIDIWINDOW_H
#define MIDIWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QSpinBox>

#include "qmidisequencer.h"

namespace Ui {
    class MidiWindow;
}

class MidiWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MidiWindow(QWidget *parent = 0);
    ~MidiWindow();

private:
    void saveConf();
    Ui::MidiWindow *ui;
    bool isLearning;

    QSpinBox* learningCC;
    QSpinBox* learningNote;
    QMidiSequencer *m_sequencer;

public slots:
    void readConf();
    void addProfil();

    void getNewNoteValue(int channel, int note, int velocity);
    void getNewControllerValue(int ccId, int value);

    void learnTimeSpeedCC();
    void learnBassTimeSpeedCC();
    void learnGammaCC();
    void learnBrightnessCC();
    void learnContrastCC();
    void learnDesaturateCC();
    void learnBlackFade();

    void learn1Effect1CC();
    void learn1Effect2CC();
    void learn1Effect3CC();
    void learn1Effect4CC();

    void learn2Effect1CC();
    void learn2Effect2CC();
    void learn2Effect3CC();
    void learn2Effect4CC();

    void learnSwitchDisplay();
    void learnSceneUp();
    void learnSceneDown();
    void learnTransitionUp();
    void learnTransitionDown();

    void setProgressSpectrum(int);
    void setProgressGamma(int);
    void setProgressWhite(int);
    void setCloneFader();

    void refreshHardware();
    void newMidiSequencer(int id);

signals:
    void timeSpeedValueChanged(int newValue);
    void bassTimeSpeedValueChanged(int newValue);
    void gammaValueChanged(int newValue);
    void brightnessValueChanged(int newValue);
    void contrastValueChanged(int newValue);
    void desaturateValueChanged(int newValue);
    void blackFadeValueChanged(int newValue);
    void CC1Changed(int a,float b);
    void CC2Changed(int a,float b);
    void switchDisplay();
    void sceneUp();
    void sceneDown();
    void transitionUp();
    void transitionDown();

};

#endif // MIDIWINDOW_H
