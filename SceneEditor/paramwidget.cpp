#include "paramwidget.h"
#include "ui_paramwidget.h"
#include "core.h"
ParamWidget::ParamWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ParamWidget)
{
    ui->setupUi(this);

    connect(ui->CC0, SIGNAL(valueChanged(int)), this, SLOT(CC0Changed(int)));
    connect(ui->CC1, SIGNAL(valueChanged(int)), this, SLOT(CC1Changed(int)));
    connect(ui->CC2, SIGNAL(valueChanged(int)), this, SLOT(CC2Changed(int)));
    connect(ui->CC3, SIGNAL(valueChanged(int)), this, SLOT(CC3Changed(int)));
}

ParamWidget::~ParamWidget()
{
    delete ui;
}

void ParamWidget::clearName()
{
    ui->label0->setText("");
    ui->label1->setText("");
    ui->label2->setText("");
    ui->label3->setText("");
}

void ParamWidget::nameCCChanged(int id, QString name)
{
    if(id == 0)
        ui->label0->setText(name);
    else if(id == 1)
        ui->label1->setText(name);
    else if(id == 2)
        ui->label2->setText(name);
    else if(id == 3)
        ui->label3->setText(name);
}

void ParamWidget::CC0Changed(int v)
{
    Core::instance()->setCC(0, float(v)/100.f);
}

void ParamWidget::CC1Changed(int v)
{
    Core::instance()->setCC(1, float(v)/100.f);
}
void ParamWidget::CC2Changed(int v)
{
    Core::instance()->setCC(2, float(v)/100.f);
}
void ParamWidget::CC3Changed(int v)
{
    Core::instance()->setCC(3, float(v)/100.f);
}
