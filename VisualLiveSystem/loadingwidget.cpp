#include "loadingwidget.h"
#include "ui_loadingwidget.h"

LoadingWidget::LoadingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoadingWidget)
{
    ui->setupUi(this);
}

LoadingWidget::~LoadingWidget()
{
    delete ui;
}

void LoadingWidget::set(int a)
{
    ui->progressBar->setValue(a);
}

void LoadingWidget::setTitle(QString t)
{
    ui->groupBox->setTitle(t);
}
