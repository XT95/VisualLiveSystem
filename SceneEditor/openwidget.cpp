#include <QDir>
#include <QTimer>

#include "openwidget.h"
#include "core.h"
#include "ui_openwidget.h"

OpenWidget::OpenWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpenWidget)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(refreshList()));
    timer->start(10000);

    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(ok()));
    refreshList();
}

OpenWidget::~OpenWidget()
{
    delete ui;
}

void OpenWidget::ok()
{
    emit open( ui->listWidget->currentItem()->text() );
    cancel();
}


void OpenWidget::cancel()
{
    hide();
}


void OpenWidget::refreshList()
{
    ui->listWidget->clear();

    QDir dir("./data/scenes");
    QStringList files = dir.entryList();
    for(int i=0; i<files.size(); i++)
    {
        QString item = files.at(i).toLocal8Bit().constData();

        if( item != "." && item != ".." && item != ".svn" )
        {
            ui->listWidget->addItem(item);
        }
    }
}
