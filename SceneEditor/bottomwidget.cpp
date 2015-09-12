#include "bottomwidget.h"
#include "ui_bottomwidget.h"

BottomWidget::BottomWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::BottomWidget)
{
    ui->setupUi(this);
    connect(ui->logWidget, SIGNAL(textChanged()), this, SLOT(logChanged()));
}

BottomWidget::~BottomWidget()
{
    delete ui;
}

QTextEdit* BottomWidget::getLogWidget()
 { return ui->logWidget; }

void BottomWidget::logChanged()
{
    ui->logWidget->ensureCursorVisible();
}
