#include "midiwindow.h"
#include "ui_midiwindow.h"
#include "core.h"
#include <QDir>
#include <QPixmap>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <portmidi.h>

#include <cmath>
#include "core.h"



MidiWindow::MidiWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MidiWindow)
{
    ui->setupUi(this);

    Pm_Initialize();
    m_sequencer = NULL;
    emit refreshHardware();
    emit newMidiSequencer( Pm_GetDefaultInputDeviceID());
    //Read profil box ...
    {
        QDir dir("./data/config/midi");
        QStringList files = dir.entryList();
        for(int i=0; i<files.size(); i++)
        {
            QString item = files.at(i).toLocal8Bit().constData();

            if( item != "." && item != ".." && item != ".svn" )
            {
                item.remove(item.length()-5,5);
                ui->profilBox->addItem(item);
            }
        }
    }
    //readConf();

    connect(ui->btnBassTimeSpeed, SIGNAL(clicked()), this, SLOT(learnBassTimeSpeedCC()));
    connect(ui->btnGamma, SIGNAL(clicked()), this, SLOT(learnGammaCC()));
    connect(ui->btnBrightness, SIGNAL(clicked()), this, SLOT(learnBrightnessCC()));
    connect(ui->btnContrast, SIGNAL(clicked()), this, SLOT(learnContrastCC()));
    connect(ui->btnDesaturate, SIGNAL(clicked()), this, SLOT(learnDesaturateCC()));
    connect(ui->btnBlackFade, SIGNAL(clicked()), this, SLOT(learnBlackFade()));
    connect(ui->btnTimeSpeed, SIGNAL(clicked()), this, SLOT(learnTimeSpeedCC()));
    connect(ui->btn1Effect1, SIGNAL(clicked()), this, SLOT(learn1Effect1CC()));
    connect(ui->btn1Effect2, SIGNAL(clicked()), this, SLOT(learn1Effect2CC()));
    connect(ui->btn1Effect3, SIGNAL(clicked()), this, SLOT(learn1Effect3CC()));
    connect(ui->btn1Effect4, SIGNAL(clicked()), this, SLOT(learn1Effect4CC()));
    connect(ui->btn2Effect1, SIGNAL(clicked()), this, SLOT(learn2Effect1CC()));
    connect(ui->btn2Effect2, SIGNAL(clicked()), this, SLOT(learn2Effect2CC()));
    connect(ui->btn2Effect3, SIGNAL(clicked()), this, SLOT(learn2Effect3CC()));
    connect(ui->btn2Effect4, SIGNAL(clicked()), this, SLOT(learn2Effect4CC()));
    connect(ui->btnSwitchDisplay, SIGNAL(clicked()), this, SLOT(learnSwitchDisplay()));
    connect(ui->btnSceneNext, SIGNAL(clicked()), this, SLOT(learnSceneUp()));
    connect(ui->btnSceneBack, SIGNAL(clicked()), this, SLOT(learnSceneDown()));
    connect(ui->btnTransitionUp, SIGNAL(clicked()), this, SLOT(learnTransitionUp()));
    connect(ui->btnTransitionDown, SIGNAL(clicked()), this, SLOT(learnTransitionDown()));
    connect(ui->cloneFaderCheck, SIGNAL(clicked()), this, SLOT(setCloneFader()));
    connect(ui->profilBox, SIGNAL(currentIndexChanged(int)), this, SLOT(readConf()));
    connect(ui->newProfilButton, SIGNAL(clicked()), this, SLOT(addProfil()));
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(refreshHardware()));
    connect(ui->hardwareComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(newMidiSequencer(int)));
    isLearning = false;
    learningCC = NULL;
}

MidiWindow::~MidiWindow()
{
    m_sequencer->terminate();
    m_sequencer->wait();
    delete m_sequencer;
    delete ui;
}

void MidiWindow::addProfil()
{
    QString name = QInputDialog::getText(this, "Profil", "Write the name of your new profil");
    ui->profilBox->addItem(name);
    ui->profilBox->setCurrentIndex(ui->profilBox->count()-1);
    QFile file("data/config/midi/"+ui->profilBox->currentText()+".conf");
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream flux(&file);
        flux.setCodec("UTF-8");
        for(int i=0; i<14; i++)
            flux << 0 << " ";
        file.close();
    }
}

void MidiWindow::readConf()
{
    QFile file("data/config/midi/"+ui->profilBox->currentText()+".conf");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&file);

    int tmp;

    flux >> tmp;
    ui->spinSwitchDisplay->setValue(tmp);

    flux >> tmp;
    ui->spinBassTimeSpeed->setValue(tmp);
    flux >> tmp;
    ui->spinGamma->setValue(tmp);
    flux >> tmp;
    ui->spinBrightness->setValue(tmp);
    flux >> tmp;
    ui->spinTimeSpeed->setValue(tmp);

    flux >> tmp;
    ui->spin1Effect1->setValue(tmp);
    flux >> tmp;
    ui->spin1Effect2->setValue(tmp);
    flux >> tmp;
    ui->spin1Effect3->setValue(tmp);
    flux >> tmp;
    ui->spin1Effect4->setValue(tmp);

    flux >> tmp;
    ui->spin2Effect1->setValue(tmp);
    flux >> tmp;
    ui->spin2Effect2->setValue(tmp);
    flux >> tmp;
    ui->spin2Effect3->setValue(tmp);
    flux >> tmp;
    ui->spin2Effect4->setValue(tmp);

    flux >> tmp;
    ui->cloneFaderCheck->setChecked((bool)tmp);
}

void MidiWindow::saveConf()
{
    QFile file("data/config/midi/"+ui->profilBox->currentText()+".conf");
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream flux(&file);
        flux.setCodec("UTF-8");

        flux << ui->spinSwitchDisplay->value() << " ";

        flux << ui->spinBassTimeSpeed->value() << " ";
        flux << ui->spinGamma->value() << " ";
        flux << ui->spinBrightness->value() << " ";
        flux << ui->spinTimeSpeed->value() << " ";

        flux << ui->spin1Effect1->value() << " ";
        flux << ui->spin1Effect2->value() << " ";
        flux << ui->spin1Effect3->value() << " ";
        flux << ui->spin1Effect4->value() << " ";

        flux << ui->spin2Effect1->value() << " ";
        flux << ui->spin2Effect2->value() << " ";
        flux << ui->spin2Effect3->value() << " ";
        flux << ui->spin2Effect4->value() << " ";

        flux << ui->cloneFaderCheck->isChecked() << " ";
        file.close();
    }
    else
    {
        QMessageBox::critical(this, "Midi profil", "Can't save the profil");
    }
}

void MidiWindow::getNewNoteValue(int channel, int note, int velocity)
{
    if(isLearning) {
        learningNote->setValue(note);
        learningNote = NULL;
        isLearning = false;
        ui->btnSwitchDisplay->setEnabled(true);
        ui->btnSceneNext->setEnabled(true);
        ui->btnSceneBack->setEnabled(true);
        ui->btnTransitionUp->setEnabled(true);
        ui->btnTransitionDown->setEnabled(true);
    }
    else
    {
        if(note == ui->spinSwitchDisplay->value())
        {
            emit switchDisplay();
        }
        else if(note == ui->spinSceneNext->value())
        {
            emit sceneUp();
        }
        else if(note == ui->spinSceneBack->value())
        {
            emit sceneDown();
        }
        else if(note == ui->spinTransitionUp->value())
        {
            emit transitionUp();
        }
        else if(note == ui->spinTransitionDown->value())
        {
            emit transitionDown();
        }
    }
    saveConf();
}

void MidiWindow::getNewControllerValue(int ccId, int value) {
    bool cloneFader = ui->cloneFaderCheck->isChecked();
    if(isLearning) {
        learningCC->setValue(ccId);
        learningCC = NULL;
        isLearning = false;

        ui->btnSwitchDisplay->setChecked(false);
        ui->btnBassTimeSpeed->setChecked(false);
        ui->btnGamma->setChecked(false);
        ui->btnBrightness->setChecked(false);
        ui->btnContrast->setChecked(false);
        ui->btnDesaturate->setChecked(false);
        ui->btnBlackFade->setChecked(false);
        ui->btnTimeSpeed->setChecked(false);

        ui->btn1Effect1->setChecked(false);
        ui->btn1Effect2->setChecked(false);
        ui->btn1Effect3->setChecked(false);
        ui->btn1Effect4->setChecked(false);

        ui->btnSwitchDisplay->setEnabled(true);
        ui->btnSceneNext->setEnabled(true);
        ui->btnSceneBack->setEnabled(true);
        ui->btnTransitionUp->setEnabled(true);
        ui->btnTransitionDown->setEnabled(true);
        ui->btnBassTimeSpeed->setEnabled(true);
        ui->btnGamma->setEnabled(true);
        ui->btnBrightness->setEnabled(true);
        ui->btnContrast->setEnabled(true);
        ui->btnDesaturate->setEnabled(true);
        ui->btnBlackFade->setEnabled(true);
        ui->btnTimeSpeed->setEnabled(true);

        ui->btn1Effect1->setEnabled(true);
        ui->btn1Effect2->setEnabled(true);
        ui->btn1Effect3->setEnabled(true);
        ui->btn1Effect4->setEnabled(true);

        if(!ui->cloneFaderCheck->isChecked())
        {

            ui->btn2Effect1->setChecked(false);
            ui->btn2Effect2->setChecked(false);
            ui->btn2Effect3->setChecked(false);
            ui->btn2Effect4->setChecked(false);

            ui->btn2Effect1->setEnabled(true);
            ui->btn2Effect2->setEnabled(true);
            ui->btn2Effect3->setEnabled(true);
            ui->btn2Effect4->setEnabled(true);
        }
    } else {
        //if(scene[actualScene].getShader() == NULL) return;
        float ccValue = ((float)value)/128.0f*100.f;

        if(ccId == ui->spinBassTimeSpeed->value())
        {
            //BassTimeSpeed = ccValue;
            emit bassTimeSpeedValueChanged((int)(ccValue));
            ui->barBassTimeSpeed->setValue((int)(ccValue));
        }
        else if(ccId == ui->spinGamma->value())
        {
            //Gamma = ccValue;
            emit gammaValueChanged((int)(ccValue));
            ui->barGamma->setValue((int)((ccValue)));
        }
        else if(ccId == ui->spinBrightness->value())
        {
            //blackFade = fmax(1.f-(ccValue*-2.+1.),.0f);
            //whiteFlash = fmax(ccValue*2.-1.,0.f);
            ui->barBrightness->setValue((int)(ccValue));
            emit brightnessValueChanged((int)(ccValue));
        }
        else if(ccId == ui->spinContrast->value())
        {
            ui->barContrast->setValue((int)(ccValue));
            emit contrastValueChanged((int)(ccValue));
        }
        else if(ccId == ui->spinDesaturate->value())
        {
            ui->barDesaturate->setValue((int)(ccValue));
            emit desaturateValueChanged((int)(ccValue));
        }
        else if(ccId == ui->spinBlackFade->value())
        {
            ui->barBlackFade->setValue((int)(ccValue));
            emit blackFadeValueChanged((int)(ccValue));
        }
        else if(ccId == ui->spinTimeSpeed->value())
        {
            //timeSpeed = ccValue;
            ui->barTimeSpeed->setValue((int)(ccValue));
            emit timeSpeedValueChanged((int)(ccValue));
        }
        else if(ccId == ui->spin1Effect1->value())
        {
            if(cloneFader)
            {
                //scene[1].setParam(0,ccValue);
                //scene[1].getShader()->sendf("CC[0]", ccValue);
                emit CC2Changed(0,ccValue);
            }
            //scene[0].setParam(0,ccValue);
            //scene[0].getShader()->sendf("CC[0]", ccValue);
            ui->bar1Effect1->setValue((int)(ccValue));
            emit CC1Changed(0,ccValue);
        }
        else if(ccId == ui->spin1Effect2->value())
        {
            if(cloneFader)
            {
                //scene[1].setParam(1,ccValue);
                //scene[1].getShader()->sendf("CC[1]", ccValue);
                emit CC2Changed(1,ccValue);
            }
            //scene[0].setParam(1,ccValue);
            //scene[0].getShader()->sendf("CC[1]", ccValue);
            ui->bar1Effect2->setValue((int)(ccValue));
            emit CC1Changed(1,ccValue);
        }
        else if(ccId == ui->spin1Effect3->value())
        {
            if(cloneFader)
            {
                //scene[1].setParam(2,ccValue);
                //scene[1].getShader()->sendf("CC[2]", ccValue);
                emit CC2Changed(2,ccValue);
            }
            //scene[0].setParam(2,ccValue);
            //scene[0].getShader()->sendf("CC[2]", ccValue);
            ui->bar1Effect3->setValue((int)(ccValue));
            emit CC1Changed(2,ccValue);
        }
        else if(ccId == ui->spin1Effect4->value())
        {
            if(cloneFader)
            {
                //scene[1].setParam(3,ccValue);
                //scene[1].getShader()->sendf("CC[3]", ccValue);
                emit CC2Changed(3,ccValue);
            }
            //scene[0].setParam(3,ccValue);
            //scene[0].getShader()->sendf("CC[3]", ccValue);
            ui->bar1Effect4->setValue((int)(ccValue));
            emit CC1Changed(3,ccValue);
        }
        else if(ccId == ui->spin2Effect1->value())
        {
            //scene[1].setParam(0,ccValue);
            //scene[1].getShader()->sendf("CC[0]", ccValue);
            ui->bar2Effect1->setValue((int)(ccValue));
            emit CC2Changed(0,ccValue);
        }
        else if(ccId == ui->spin2Effect2->value())
        {
            //scene[1].setParam(1,ccValue);
            //scene[1].getShader()->sendf("CC[1]", ccValue);
            ui->bar2Effect2->setValue((int)(ccValue));
            emit CC2Changed(1,ccValue);
        }
        else if(ccId == ui->spin2Effect3->value())
        {
            //scene[1].setParam(2,ccValue);
            //scene[1].getShader()->sendf("CC[2]", ccValue);
            ui->bar2Effect3->setValue((int)(ccValue));
            emit CC2Changed(2,ccValue);
        }
        else if(ccId == ui->spin2Effect4->value())
        {
            //scene[1].setParam(3,ccValue);
            //scene[1].getShader()->sendf("CC[3]", ccValue);
            ui->bar2Effect4->setValue((int)(ccValue));
            emit CC2Changed(3,ccValue);
        }
    }
}

void MidiWindow::learnBassTimeSpeedCC() {
    isLearning = true;
    ui->btnBassTimeSpeed->setEnabled(false);
    learningCC = ui->spinBassTimeSpeed;
}

void MidiWindow::learnGammaCC() {
    isLearning = true;
    ui->btnGamma->setEnabled(false);
    learningCC = ui->spinGamma;
}

void MidiWindow::learnBrightnessCC() {
    isLearning = true;
    ui->btnBrightness->setEnabled(false);
    learningCC = ui->spinBrightness;
}

void MidiWindow::learnContrastCC() {
    isLearning = true;
    ui->btnContrast->setEnabled(false);
    learningCC = ui->spinContrast;
}
void MidiWindow::learnDesaturateCC() {
    isLearning = true;
    ui->btnDesaturate->setEnabled(false);
    learningCC = ui->spinDesaturate;
}
void MidiWindow::learnBlackFade() {
    isLearning = true;
    ui->btnBlackFade->setEnabled(false);
    learningCC = ui->spinBlackFade;
}
void MidiWindow::learnTimeSpeedCC() {
    isLearning = true;
    ui->btnTimeSpeed->setEnabled(false);
    learningCC = ui->spinTimeSpeed;
}



void MidiWindow::learn1Effect1CC() {
    isLearning = true;
    ui->btn1Effect1->setEnabled(false);
    learningCC = ui->spin1Effect1;
}

void MidiWindow::learn1Effect2CC() {
    isLearning = true;
    ui->btn1Effect2->setEnabled(false);
    learningCC = ui->spin1Effect2;
}

void MidiWindow::learn1Effect3CC() {
    isLearning = true;
    ui->btn1Effect3->setEnabled(false);
    learningCC = ui->spin1Effect3;
}

void MidiWindow::learn1Effect4CC() {
    isLearning = true;
    ui->btn1Effect4->setEnabled(false);
    learningCC = ui->spin1Effect4;
}



void MidiWindow::learn2Effect1CC() {
    isLearning = true;
    ui->btn2Effect1->setEnabled(false);
    learningCC = ui->spin2Effect1;
}

void MidiWindow::learn2Effect2CC() {
    isLearning = true;
    ui->btn2Effect2->setEnabled(false);
    learningCC = ui->spin2Effect2;
}

void MidiWindow::learn2Effect3CC() {
    isLearning = true;
    ui->btn2Effect3->setEnabled(false);
    learningCC = ui->spin2Effect3;
}

void MidiWindow::learn2Effect4CC() {
    isLearning = true;
    ui->btn2Effect4->setEnabled(false);
    learningCC = ui->spin2Effect4;
}



void MidiWindow::learnSwitchDisplay() {
    isLearning = true;
    ui->btnSwitchDisplay->setEnabled(false);
    learningNote = ui->spinSwitchDisplay;
}

void MidiWindow::learnSceneUp() {
    isLearning = true;
    ui->btnSceneNext->setEnabled(false);
    learningNote = ui->spinSceneNext;
}
void MidiWindow::learnSceneDown() {
    isLearning = true;
    ui->btnSceneBack->setEnabled(false);
    learningNote = ui->spinSceneBack;
}

void MidiWindow::learnTransitionUp() {
    isLearning = true;
    ui->btnTransitionUp->setEnabled(false);
    learningNote = ui->spinTransitionUp;
}
void MidiWindow::learnTransitionDown() {
    isLearning = true;
    ui->btnTransitionDown->setEnabled(false);
    learningNote = ui->spinTransitionDown;
}
void MidiWindow::setProgressSpectrum(int x) {
    getNewControllerValue(ui->spinBassTimeSpeed->value(), x);
}


void MidiWindow::setProgressGamma(int x) {
    isLearning = false;
    getNewControllerValue(ui->spinGamma->value(), x);
}


void MidiWindow::setProgressWhite(int x) {
    getNewControllerValue(ui->spinBrightness->value(), x);

}



void MidiWindow::setCloneFader()
{
    bool cloneFader = ui->cloneFaderCheck->isChecked();
    if(cloneFader)
    {
        ui->label_24->setEnabled(false);
        ui->label_25->setEnabled(false);
        ui->label_26->setEnabled(false);
        ui->label_27->setEnabled(false);
        ui->btn2Effect1->setEnabled(false);
        ui->btn2Effect2->setEnabled(false);
        ui->btn2Effect3->setEnabled(false);
        ui->btn2Effect4->setEnabled(false);
        ui->spin2Effect1->setEnabled(false);
        ui->spin2Effect2->setEnabled(false);
        ui->spin2Effect3->setEnabled(false);
        ui->spin2Effect4->setEnabled(false);
        ui->bar2Effect1->setEnabled(false);
        ui->bar2Effect2->setEnabled(false);
        ui->bar2Effect3->setEnabled(false);
        ui->bar2Effect4->setEnabled(false);
    }
    else
    {
        ui->label_24->setEnabled(true);
        ui->label_25->setEnabled(true);
        ui->label_26->setEnabled(true);
        ui->label_27->setEnabled(true);
        ui->btn2Effect1->setEnabled(true);
        ui->btn2Effect2->setEnabled(true);
        ui->btn2Effect3->setEnabled(true);
        ui->btn2Effect4->setEnabled(true);
        ui->spin2Effect1->setEnabled(true);
        ui->spin2Effect2->setEnabled(true);
        ui->spin2Effect3->setEnabled(true);
        ui->spin2Effect4->setEnabled(true);
        ui->bar2Effect1->setEnabled(true);
        ui->bar2Effect2->setEnabled(true);
        ui->bar2Effect3->setEnabled(true);
        ui->bar2Effect4->setEnabled(true);
    }
}

void MidiWindow::refreshHardware()
{
    ui->hardwareComboBox->clear();
    for(int i=0; i<Pm_CountDevices(); i++)
    {
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
        QString item = info->name;

        if( info->input )
        {
            ui->hardwareComboBox->addItem(item);
        }
        else
        {
            ui->hardwareComboBox->addItem(" ");
        }
    }
}

void MidiWindow::newMidiSequencer(int id)
{
    if(m_sequencer)
    {
        m_sequencer->terminate();
        m_sequencer->wait();
        delete m_sequencer;
    }
    m_sequencer = new QMidiSequencer("Visual Live System", id, this);
    connect(m_sequencer, SIGNAL(controllerValueChanged(int,int)), this, SLOT(getNewControllerValue(int,int)));
    connect(m_sequencer, SIGNAL(noteOn(int,int,int)), this, SLOT(getNewNoteValue(int,int,int)));
    m_sequencer->start();
}
