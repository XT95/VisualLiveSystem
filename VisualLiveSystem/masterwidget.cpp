#include <cmath>
#include "masterwidget.h"
#include "ui_masterwidget.h"

#include "core.h"

MasterWidget::MasterWidget(QGLWidget *share, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::MasterWidget)
{
    ui->setupUi(this);


    m_audiovisu = new AudioVisualizer(share,parent);
    ui->layout->addWidget(m_audiovisu);


    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(Switch()));
    connect(ui->blackWhite, SIGNAL(valueChanged(int)), this, SLOT(postProcessingChanged()));
    connect(ui->motionBlur, SIGNAL(valueChanged(int)), this, SLOT(postProcessingChanged()));
    connect(ui->gamma, SIGNAL(valueChanged(int)), this, SLOT(postProcessingChanged()));
    connect(ui->contrast, SIGNAL(valueChanged(int)), this, SLOT(postProcessingChanged()));
    connect(ui->desaturate, SIGNAL(valueChanged(int)), this, SLOT(postProcessingChanged()));
    connect(ui->brightness , SIGNAL(valueChanged(int)), this, SLOT(postProcessingChanged()));
    connect(ui->fisheye, SIGNAL(valueChanged(int)), this, SLOT(postProcessingChanged()));
    connect(ui->timeSpeed, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged()));
    connect(ui->bassTime, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged()));
    connect(ui->bassMotionSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged()));

    connect(ui->bassCutoffSlider, SIGNAL(valueChanged(int)), this, SLOT(basscutOffChanged(int)));
    connect(ui->transitionBox, SIGNAL(currentRowChanged(int)), this, SLOT(transitionChanged(int)));
    connect(Core::instance(),SIGNAL(pow2Changed(uint)),this,SLOT(pow2Changed(uint)));

    ui->bassCutoffSpin->setValue((double)Core::instance()->bassCutoff*((double)Signal::frequency/Core::instance()->pow2Size));
    ui->bassCutoffSlider->setValue(Core::instance()->bassCutoff);
    {
        QDir dir("./data/transitions");
        QStringList files = dir.entryList();
        for(int i=0; i<files.size(); i++)
        {
            QString item = files.at(i).toLocal8Bit().constData();

            if( item != "." && item != ".." && item != ".svn" )
            {
                ui->transitionBox->addItem(item);
            }
        }
    }

    m_timer.restart();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateInfo()));
    timer->start(500);


}

MasterWidget::~MasterWidget()
{
    delete ui;
}


void MasterWidget::Switch()
{
    emit transition();
}

void MasterWidget::pow2Changed(unsigned int p)
{
    ui->bassCutoffSpin->setValue((double)Core::instance()->bassCutoff*(double)Signal::frequency/(double)(p));
}

void MasterWidget::postProcessingChanged()
{
    float black = float(ui->blackWhite->value())/100.;
    float motionBlur = float(ui->motionBlur->value())/100.;
    float gamma = float(ui->gamma->value())/100.*3.5+1.;
    float contrast = float(ui->contrast->value())/200.;
    float desaturate = float(ui->desaturate->value())/100.;
    float brightness = float(ui->brightness->value())/100.*3.;
    float fisheye = float(ui->fisheye->value())/100.;
    emit finalPostFXDataChanged(black, motionBlur, contrast, gamma, desaturate, brightness, fisheye);
}

void MasterWidget::sliderChanged()
{
    Core::instance()->setTimeSpeed(float(ui->timeSpeed->value())*4./100.);
    Core::instance()->setBassTimeSpeed(float(ui->bassTime->value())*4./100.);
    float bassMotion = float(ui->bassMotionSlider->value())/100.;
    Core::instance()->setBassMotion(bassMotion);
}

void MasterWidget::updateInfo()
{
    QString string;
    unsigned int t = m_timer.elapsed()/1000;


    ui->infoChanA->setText(QString::number(Core::instance()->infoChannelA)+QString(" ms"));
    ui->infoChanB->setText(QString::number(Core::instance()->infoChannelB)+QString(" ms"));

    ui->infoPostFX->setText(QString::number(Core::instance()->infoPostFX)+QString(" ms"));

    ui->infoFPS->setText(QString::number(Core::instance()->infoFPS));
    if(Core::instance()->infoFPS>30)
        ui->infoFPS->setStyleSheet("QLabel { color : green; }");
    else if(Core::instance()->infoFPS>24)
        ui->infoFPS->setStyleSheet("QLabel { color : orange; }");
    else
        ui->infoFPS->setStyleSheet("QLabel { color : red; }");


    ui->infoSound->setText(QString::number(Core::instance()->infoSound)+QString(" ms"));


    //Time
    string = QString::number(t/60);
    string += " min ";
    string += QString::number(t%60);
    string += " sec";
    ui->infoTime->setText(string);
}

void MasterWidget::inLive(bool t)
{
    m_timer.restart();
}



void MasterWidget::gammaChanged(int v)
{
    ui->gamma->setValue(v);
}

void MasterWidget::brightnessChanged(int v)
{
    ui->brightness->setValue(v);
}
void MasterWidget::contrastChanged(int v)
{
    ui->contrast->setValue(v);
}
void MasterWidget::desaturateChanged(int v)
{
    ui->desaturate->setValue(v);
}
void MasterWidget::blackFadeChanged(int v)
{
    ui->blackWhite->setValue(v);
}

void MasterWidget::timeSpeedChanged(int v)
{
    ui->timeSpeed->setValue(v);
}

void MasterWidget::bassTimeSpeedChanged(int v)
{
    ui->bassTime->setValue(v);
}

void MasterWidget::transitionUp()
{
    int n = ui->transitionBox->currentRow()-1;
    if(n == -1)
        n = ui->transitionBox->count()-1;
    ui->transitionBox->setCurrentRow(n);
    Core::instance()->setCurrentTransition(n);
}

void MasterWidget::transitionDown()
{
    int n = ui->transitionBox->currentRow()+1;
    if(n == ui->transitionBox->count())
        n = 0;
    ui->transitionBox->setCurrentRow(n);
    Core::instance()->setCurrentTransition(n);
}

void MasterWidget::transitionChanged(int n)
{
    Core::instance()->setCurrentTransition(n);
}

void MasterWidget::basscutOffChanged(int i)
{
    Core::instance()->bassCutoff = i;
    ui->bassCutoffSpin->setValue((double)i*((double)Signal::frequency/(double)Core::instance()->pow2Size));
}
