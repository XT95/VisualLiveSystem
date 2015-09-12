#include "visualmanager.h"
#include "ui_visualmanager.h"
#include "core.h"
#include <QDir>
#include <QPixmap>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>


VisualManager::VisualManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisualManager)
{
    ui->setupUi(this);


    getSceneList();
    getPostFXList();

    //Connections
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->deleteAButton, SIGNAL(clicked()), this, SLOT(delToA()));
    connect(ui->deleteBButton, SIGNAL(clicked()), this, SLOT(delToB()));
    connect(ui->deletePostButton, SIGNAL(clicked()), this, SLOT(delPost()));
    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(resolutionChanged()));
    connect(ui->spinBox_2, SIGNAL(valueChanged(int)), this, SLOT(resolutionChanged()));
    connect(ui->refreshScene, SIGNAL(clicked()), this, SLOT(getSceneList()));
    connect(ui->refreshPostFX, SIGNAL(clicked()), this, SLOT(getPostFXList()));
}

VisualManager::~VisualManager()
{
    delete ui;
}

void VisualManager::resolutionChanged()
{
    Core::instance()->setResolution( ui->spinBox->value(),ui->spinBox_2->value());
}

void VisualManager::delToA()
{
    delete ui->chanAList->currentItem() ;
}


void VisualManager::delToB()
{
    delete ui->chanBList->currentItem() ;
}

void VisualManager::delPost()
{
    delete ui->FXList->currentItem() ;
}

void VisualManager::getSceneList()
{
    ui->sceneList->clear();

    QDir dir("./data/scenes");
    QStringList files = dir.entryList();
    for(int i=0; i<files.size(); i++)
    {
        QString item = files.at(i).toLocal8Bit().constData();

        if( item != "." && item != ".." && item != ".svn" )
        {
            ui->sceneList->addItem(item);
        }
    }

}

void VisualManager::getPostFXList()
{
    ui->postFXList->clear();

    QDir dir("./data/postFX");
    QStringList files = dir.entryList();
    for(int i=0; i<files.size(); i++)
    {
        QString item = files.at(i).toLocal8Bit().constData();

        if( item != "." && item != ".." && item != ".svn" )
        {
            ui->postFXList->addItem(item);
        }
    }
}

void VisualManager::apply()
{
    QList<QListWidgetItem *> list;
    for(int i=0; i<ui->chanAList->count(); i++)
    {
        list.push_back(ui->chanAList->item(i));
    }
    emit newListChanA(list);

    list.clear();
    for(int i=0; i<ui->chanBList->count(); i++)
    {
        list.push_back(ui->chanBList->item(i));
    }
    emit newListChanB(list);

    list.clear();
    for(int i=0; i<ui->FXList->count(); i++)
    {
        list.push_back(ui->FXList->item(i));
    }
    emit newListFX(list);

    Core::instance()->Switch();
    if(Core::instance()->getCurrentChannel() != 0)
        Core::instance()->Switch();
}
