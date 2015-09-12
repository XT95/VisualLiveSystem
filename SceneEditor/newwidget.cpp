#include <QDir>
#include <QFile>
#include <QMessageBox>

#include "newwidget.h"
#include "ui_newwidget.h"

NewWidget::NewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewWidget)
{
    ui->setupUi(this);
    connect(ui->buttonOK, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui->buttonCancel, SIGNAL(clicked()), this, SLOT(cancel()));
}

NewWidget::~NewWidget()
{
    delete ui;
}

void NewWidget::ok()
{
    if( QDir("./data/scenes/"+ui->name->text()).exists() )
    {
        QMessageBox::critical(NULL, "Error", "The scene "+ui->name->text()+" already exist.");
    }
    else
    {
        QDir().mkdir("./data/scenes/"+ui->name->text());
        if( ui->empty->isChecked() )
        {
            QFile::copy(":res/framework/Empty/config.xml", "./data/scenes/"+ui->name->text()+"/config.xml");
            QFile::copy(":res/framework/Empty/Scene.glsl", "./data/scenes/"+ui->name->text()+"/Scene.glsl");
        }
        else if( ui->df->isChecked() )
        {
            QFile::copy(":res/framework/DF/config.xml", "./data/scenes/"+ui->name->text()+"/config.xml");
            QFile::copy(":res/framework/DF/Scene.glsl", "./data/scenes/"+ui->name->text()+"/Scene.glsl");
        }
        else if( ui->raytrace->isChecked() )
        {
            QFile::copy(":res/framework/Raytracing/config.xml", "./data/scenes/"+ui->name->text()+"/config.xml");
            QFile::copy(":res/framework/Raytracing/Scene.glsl", "./data/scenes/"+ui->name->text()+"/Scene.glsl");
        }
        emit open(ui->name->text());
        cancel();
    }

}


void NewWidget::cancel()
{
    close();
}
