#include <QFileDialog>
#include <QTimer>
#include <QDialog>
#include <cmath>
#include <iostream>
#include <QThread>
#include <QMessageBox>

#include "core.h"
#include "soundmanager.h"
#include "ui_soundmanager.h"

PlaylistWidget::PlaylistWidget(QWidget *parent) :
    QListWidget(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDefaultDropAction(Qt::MoveAction);
}

bool PlaylistWidget::dropMimeData(int index,const QMimeData* data, Qt::DropAction action )
{
    if (data->hasUrls()) {
        QList<QUrl> urls = data->urls();
        for (unsigned int i=0; i < urls.size();i++){
            if (urls[i].isLocalFile())
                insertItem(index,urls[i].toLocalFile());
            else
                insertItem(index,urls[i].toString());
        }
        return true;
    }
    return false;
}

QStringList PlaylistWidget::mimeTypes() const {
  QStringList list;
  list.append("text/uri-list");
  return list;
}


SoundManager::SoundManager(AudioPipe &pipe, QWidget *parent) :
    QWidget(parent),
    _pipe(&pipe),
    ui(new Ui::SoundManager)
{
    ui->setupUi(this);

    //Connections
    connect(ui->buttonCancel, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->buttonApply, SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->buttonOk, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui->buttonRefresh, SIGNAL(clicked()), this, SLOT(refreshDeviceList()));
    connect(ui->refreshRateSlider,SIGNAL(valueChanged(int)),this,SLOT(ackApply()));
    connect(ui->refreshRateSpinBox,SIGNAL(valueChanged(int)),this,SLOT(ackApply()));
    connect(ui->frameBasedBox,SIGNAL(clicked()),this,SLOT(ackApply()));
    connect(ui->buttonRefresh, SIGNAL(clicked()), this, SLOT(ackApply()));
    connect(ui->listInputDevices,SIGNAL(currentRowChanged(int)),this,SLOT(ackApply()));
    connect(ui->listOutputDevices,SIGNAL(currentRowChanged(int)),this,SLOT(ackApply()));
    connect(_pipe,SIGNAL(computedLatency(uint)),this,SLOT(receivedLatency(unsigned int)));
    connect(ui->playlistBox,SIGNAL(clicked(bool)),this,SLOT(enableMixer(bool)));
    connect(ui->playlistWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(playTrack()));
    connect(Core::instance()->decoder,SIGNAL(trackFinished()),this,SLOT(nextTrack()));
    connect(_pipe,SIGNAL(started()),this,SLOT(audioThreadRunning()));
    connect(_pipe,SIGNAL(finished()),this,SLOT(audioThreadStopped()));
    ui->frameBasedBox->setChecked(Core::instance()->settings.value("Signal/UseFramerateMethod",true).toBool());
    ui->refreshRateSlider->setValue(Core::instance()->settings.value("Signal/RefreshRate",50).toFloat());
    ui->refreshRateSpinBox->setValue(Core::instance()->settings.value("Signal/RefreshRate",50).toFloat());
    switch (Core::instance()->pow2Size)
    {
        case 256:
            ui->pow2Box->setCurrentIndex(0);
            ui->frameBasedBox->setEnabled(false);
            break;
        case 1024:
            ui->pow2Box->setCurrentIndex(1);
            break;
        case 4096:
            ui->pow2Box->setCurrentIndex(2);
            break;
        default:
            ui->pow2Box->setCurrentIndex(1);
            break;
    }
    connect(ui->pow2Box,SIGNAL(currentIndexChanged(int)),this,SLOT(pow2SizeChanged()));
    refreshApis();
}


SoundManager::~SoundManager()
{
    delete ui;
}

void SoundManager::pow2SizeChanged()
{
    if (ui->pow2Box->currentText().toUInt() < 1024)
    {
        ui->frameBasedBox->setEnabled(false);
    }
    else {
        ui->frameBasedBox->setEnabled(true);
    }

    if (Signal::isPow2 || ui->pow2Box->currentText().toUInt() < 1024)
    {
        ackApply();
    }
    else {
        Core::instance()->setPow2Size(ui->pow2Box->currentText().toUInt());
    }
}

void SoundManager::setAudioPipe(AudioPipe& pipe)
{
  disconnect(_pipe,SIGNAL(computedLatency(uint)),this,SLOT(receivedLatency(unsigned int)));
  disconnect(_pipe,SIGNAL(started()),this,SLOT(audioThreadRunning()));
  disconnect(_pipe,SIGNAL(finished()),this,SLOT(audioThreadStopped()));

  _pipe->stop();
  _pipe=&pipe;

  connect(_pipe,SIGNAL(computedLatency(uint)),this,SLOT(receivedLatency(unsigned int)));
  connect(_pipe,SIGNAL(started()),this,SLOT(audioThreadRunning()));
  connect(_pipe,SIGNAL(finished()),this,SLOT(audioThreadStopped()));
  refreshDeviceList();
}

void SoundManager::refreshApis()
{
    disconnect(ui->apiComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(apiChanged(QString)));
    std::vector<RtAudio::Api> apis;
    RtAudio::getCompiledApi(apis);
    unsigned int index=0;
    for (unsigned int i=0; i < apis.size();i++){
        ui->apiComboBox->addItem(RtAudioApiToQString(apis[i]),apis[i]);
        if (_pipe->getCurrentApi()==apis[i]) index=i;
    }
    ui->apiComboBox->setCurrentIndex(index);
    connect(ui->apiComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(apiChanged(QString)));
}

void SoundManager::audioThreadRunning(){
    ui->audioThreadStatus->setText("Running");
    ui->audioThreadStatus->setStyleSheet("color: rgb(0, 130, 0);");
}

void SoundManager::audioThreadStopped() {
    ui->audioThreadStatus->setText("Stopped");
    ui->audioThreadStatus->setStyleSheet("color: rgb(130, 0, 0);");
}

void SoundManager::apiChanged(QString api)
{
  _pipe->setApi(QStringToRtAudioApi(api));
  refreshDeviceList();
  ackApply();
}

void SoundManager::cancel()
{
    this->close();
}
void SoundManager::apply()
{

    if (ui->buttonApply->isEnabled()){
      startRecord();
    }
    Core::instance()->settings.setValue("Audio/Driver",RtAudioApiToQString(_pipe->getCurrentApi()));
    Core::instance()->settings.setValue("Audio/DeviceOutId",_outputTable[ui->listOutputDevices->currentRow()]);
    Core::instance()->settings.setValue("Audio/DeviceInId",_inputTable[ui->listInputDevices->currentRow()]);
    Core::instance()->settings.setValue("Signal/UseFramerateMethod",ui->frameBasedBox->isChecked());
    if (ui->frameBasedBox->isChecked())
        Core::instance()->settings.setValue("Signal/RefreshRate",ui->refreshRateSlider->value());
}
void SoundManager::ok()
{
    apply();
    cancel();
}

void SoundManager::ackApply()
{
    ui->buttonApply->setEnabled(true);
}

void SoundManager::nextTrack() {

  if (ui->playlistWidget->count()) {
      unsigned int row = ui->playlistWidget->currentRow();
      row++;
      if (row >= ui->playlistWidget->count()) row = 0;
      ui->playlistWidget->setCurrentRow(row);
      playTrack();
  }
}

void SoundManager::playTrack()
{
    if (ui->playlistWidget->count()) {
        _pipe->getMixerMutex().lock();
        Core::instance()->decoder->open(ui->playlistWidget->currentItem()->text());
        _pipe->getMixerMutex().unlock();
    }
}

void SoundManager::refreshDeviceList()
{
    ui->listInputDevices->clear();
    ui->listOutputDevices->clear();
    _inputTable.clear();
    _outputTable.clear();

    _pipe->stop();

    unsigned int devices = _pipe->getDeviceCount();
    RtAudio::DeviceInfo info;
    for ( unsigned int i=0; i<devices; i++ )
    {
        info =  _pipe->getDeviceInfo( i );
        if ( info.probed == true )
        {
            if (info.inputChannels) {
                ui->listInputDevices->addItem( QString( info.name.c_str() )+" ("+QString::number(info.inputChannels)+" channels)" );
                _inputTable.append(i);
            }
            if (info.outputChannels) {
                ui->listOutputDevices->addItem( QString( info.name.c_str() )+" ("+QString::number(info.outputChannels)+" channels)" );
                _outputTable.append(i);
            }
        }
    }
    if(_inputTable.size() > 0 && _outputTable.size() > 0) {
        ui->listOutputDevices->setCurrentRow(0);
        ui->listInputDevices->setCurrentRow(0);
    }
    else if (_inputTable.size()) {
        QMessageBox::critical(0,"Fatal Error","No output device available !",QMessageBox::Ok,0);
    }
    else if (_outputTable.size()) {
        QMessageBox::critical(0,"Fatal Error","No input device available !",QMessageBox::Ok,0);
    }
    else {
        QMessageBox::critical(0,"Fatal Error","No audio device available !",QMessageBox::Ok,0);
    }
    ackApply();

}

void SoundManager::receivedLatency(unsigned int l)
{
    ui->latencyEdit->setText(QString::number(l*1000/Signal::frequency) +  " ms");
}

void SoundManager::enableMixer(bool e) {
    if (e)
        _pipe->setMixer(Core::instance()->mixer);
    else
        _pipe->setMixer(0);
}

void SoundManager::startRecord()
{
    _pipe->stop();
    _pipe->setInputDeviceId(_inputTable[ui->listInputDevices->currentRow()]);
    _pipe->setOutputDeviceId(_outputTable[ui->listOutputDevices->currentRow()]);

    Core::instance()->setPow2Size(ui->pow2Box->currentText().toUInt());
    if (ui->frameBasedBox->isChecked() && Core::instance()->pow2Size >= 1024)
        Signal::globalConfigurationFromFramerate(Signal::frequency,ui->refreshRateSlider->value());
    else
        Signal::globalConfigurationFromPow2(Signal::frequency,Core::instance()->pow2Size);

    _pipe->prepare();
    _pipe->start();
    ui->buttonApply->setEnabled(false);
    Core::instance()->resetBassTime();
}
