#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

#include "core.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bottomwidget.h"
#include "paramwidget.h"
#include "editorwidget.h"
#include "newwidget.h"
#include "openwidget.h"
#include "newshader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_info = new BottomWidget(this);
    m_param = new ParamWidget(this);
    m_editor = new EditorWidget(this);

    m_newShader = new NewShader();
    m_newShader->hide();

    m_open = new OpenWidget();
    m_open->hide();

    Core::instance()->setLogWidget( m_info->getLogWidget() );
    ui->toolBar->addAction(ui->actionNew);
    ui->toolBar->addAction(ui->actionOpen);
    ui->toolBar->addAction(ui->actionSave);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionReset_camera);
    ui->toolBar->addAction(ui->actionTake_screenshot);
    ui->toolBar->addAction(ui->actionProjector_output);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionAdd_shader);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionReload);

    addDockWidget(Qt::BottomDockWidgetArea, m_info);
    addDockWidget(Qt::LeftDockWidgetArea, m_editor);
    addDockWidget(Qt::BottomDockWidgetArea, m_param);

    connect(ui->actionNew,SIGNAL(triggered()), this, SLOT(newScene()));
    connect(ui->actionOpen,SIGNAL(triggered()), m_open, SLOT(show()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAdd_shader, SIGNAL(triggered()), m_newShader, SLOT(show()));
    connect(ui->actionReset_camera, SIGNAL(triggered()), ui->widget, SLOT(resetCamera()));
    connect(ui->actionTake_screenshot, SIGNAL(triggered()), ui->widget, SLOT(takeScreenshot()));
    connect(m_editor, SIGNAL(recompil(QString,int)), ui->widget, SLOT(recompilShader( QString, int)));
    connect(m_editor, SIGNAL(nameCCChanged(int,QString)), m_param, SLOT(nameCCChanged(int,QString)));
    connect(m_open, SIGNAL(open(QString)), this, SLOT(open(QString)));

    connect(m_newShader, SIGNAL(reload()), this, SLOT(reloadEffect()));

    connect(ui->actionReload, SIGNAL(triggered()), this, SLOT(reloadEffect()));
}

MainWindow::~MainWindow()
{
    delete m_info;
    delete ui;
}




void MainWindow::open(QString filename)
{
    Core::instance()->setEffectName( filename );
    m_param->clearName();
    ui->widget->loadScene(filename);
    m_editor->loadScene(filename);
}

void MainWindow::reloadEffect()
{
    m_param->clearName();
    ui->widget->loadScene( Core::instance()->getEffectName() );
    m_editor->loadScene( Core::instance()->getEffectName() );

}

void MainWindow::save()
{
    m_editor->save();
}

void MainWindow::newScene()
{
    NewWidget *w = new NewWidget();
    w->show();
    connect(w, SIGNAL(open(QString)), this, SLOT(open(QString)));
}

void MainWindow::newShader()
{

}
