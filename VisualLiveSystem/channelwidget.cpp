#include "channelwidget.h"
#include "ui_channelwidget.h"
#include "glscene.h"
#include "core.h"


ChannelWidget::ChannelWidget(int id, QGLWidget *share, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ChannelWidget),
    m_id(id)
{
    ui->setupUi(this);
    m_glscene = new GLScene(share,id,parent,m_id);
    m_scene = NULL;

    if(id==0)
        this->setWindowTitle( "Channel A");
    else
        this->setWindowTitle( "Channel B");
    ui->groupBox_3->layout()->addWidget(m_glscene);

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(selectChanged(int)));
    connect(ui->CC0Bar, SIGNAL(valueChanged(int)), this, SLOT(CCChanged()));
    connect(ui->CC1Bar, SIGNAL(valueChanged(int)), this, SLOT(CCChanged()));
    connect(ui->CC2Bar, SIGNAL(valueChanged(int)), this, SLOT(CCChanged()));
    connect(ui->CC3Bar, SIGNAL(valueChanged(int)), this, SLOT(CCChanged()));
}

ChannelWidget::~ChannelWidget()
{
    delete ui;
}

void ChannelWidget::updateList( QList<QListWidgetItem *> list )
{
    Core::instance()->getLoadingWidget()->show();
    ui->listWidget->clear();
    //Copy !
    if(list.size())
    {
       foreach(QListWidgetItem *i, list)
       {
          ui->listWidget->addItem(i->text());
       }
    }

    if(m_scene)
    {
        delete[] m_scene;
        m_scene = NULL;
    }
    if(!list.isEmpty())
    {
        m_scene = new Scene[list.size()];
        m_glscene->compilScenes(m_scene,list);
        selectChanged(0);
    }
}

void ChannelWidget::Switch()
{
    if( Core::instance()->getCurrentChannel() == m_id && Core::instance()->inLive() )
        ui->listWidget->setEnabled(false);
    else
        ui->listWidget->setEnabled(true);
}

void ChannelWidget::selectChanged(int a)
{
    if(m_id == Core::instance()->getCurrentChannel() && Core::instance()->inLive())
        return;

    if( Core::instance()->getLoadingWidget()->isHidden() )
    {
        m_glscene->setCurrent(a);
        m_scene[a].resetTime();
        ui->param0Label->setText( m_scene[a].getParamName(0) );
        ui->param1Label->setText( m_scene[a].getParamName(1) );
        ui->param2Label->setText( m_scene[a].getParamName(2) );
        ui->param3Label->setText( m_scene[a].getParamName(3) );
    }
}

void ChannelWidget::CCChanged(int id, float v)
{
    if(id == 0)
    {
        ui->CC0Bar->setValue((int)(v));
        Core::instance()->setCC(m_id,0,float(v)/100.f);
    }
    else if(id == 1)
    {
        ui->CC1Bar->setValue((int)(v));
        Core::instance()->setCC(m_id,1,float(v)/100.f);
    }
    else if(id == 2)
    {
        ui->CC2Bar->setValue((int)(v));
        Core::instance()->setCC(m_id,2,float(v)/100.f);
    }
    else if(id == 3)
    {
        ui->CC3Bar->setValue((int)(v));
        Core::instance()->setCC(m_id,3,float(v)/100.f);
    }
}

void ChannelWidget::CCChanged()
{

    Core::instance()->setCC(m_id,0,float(ui->CC0Bar->value())/100.f);
    Core::instance()->setCC(m_id,1,float(ui->CC1Bar->value())/100.f);
    Core::instance()->setCC(m_id,2,float(ui->CC2Bar->value())/100.f);
    Core::instance()->setCC(m_id,3,float(ui->CC3Bar->value())/100.f);
}

void ChannelWidget::sceneUp()
{
    if( ( Core::instance()->getCurrentChannel() != m_id && Core::instance()->inLive() ) || !Core::instance()->inLive() )
    {
        int n = (ui->listWidget->currentRow()+1)%ui->listWidget->count();
        ui->listWidget->setCurrentRow(n);
    }

}

void ChannelWidget::sceneDown()
{

    if( ( Core::instance()->getCurrentChannel() != m_id && Core::instance()->inLive() ) || !Core::instance()->inLive() )
    {
        int n = ui->listWidget->currentRow()-1;
        if(n == -1)
            n = ui->listWidget->count()-1;
        ui->listWidget->setCurrentRow(n);
    }
}
