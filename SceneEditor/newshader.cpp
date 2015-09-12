#include <QInputDialog>
#include <QMessageBox>
#include <QtXml>
#include <QFileDialog>
#include <QFile>

#include "newshader.h"
#include "core.h"
#include "ui_newshader.h"

NewShader::NewShader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::newShader)
{
    ui->setupUi(this);

    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(ok()));

    connect(ui->chan1ImageButton, SIGNAL(clicked()), this, SLOT(setImageChan1()));
    connect(ui->chan2ImageButton, SIGNAL(clicked()), this, SLOT(setImageChan2()));
    connect(ui->chan3ImageButton, SIGNAL(clicked()), this, SLOT(setImageChan3()));
    connect(ui->chan4ImageButton, SIGNAL(clicked()), this, SLOT(setImageChan4()));


    connect(ui->chan1PassButton, SIGNAL(clicked()), this, SLOT(setPassChan1()));
    connect(ui->chan2PassButton, SIGNAL(clicked()), this, SLOT(setPassChan2()));
    connect(ui->chan3PassButton, SIGNAL(clicked()), this, SLOT(setPassChan3()));
    connect(ui->chan4PassButton, SIGNAL(clicked()), this, SLOT(setPassChan4()));
}

NewShader::~NewShader()
{
    delete ui;
}

void NewShader::show()
{
    ui->nameLineEdit->clear();
    ui->ID->setValue(0);
    ui->type->setCurrentIndex(0);
    ui->display->setCurrentIndex(0);

    ui->chan1LineEdit->clear();
    ui->chan2LineEdit->clear();
    ui->chan3LineEdit->clear();
    ui->chan4LineEdit->clear();

    ui->chan1Name->clear();
    ui->chan2Name->clear();
    ui->chan3Name->clear();
    ui->chan4Name->clear();

    QWidget::show();
}


bool NewShader::writeDomChannel(QLineEdit *name, QLineEdit *value, QDomNode *node, QDomDocument &dom, int id)
{
    QDomElement *child = new QDomElement(dom.createElement(QString("channel")));
    if( value->text().left(6) == "Layer " )
    {
        child->setAttribute("type", "layer");
        if( name->text() == "" )
        {
            QMessageBox::warning(NULL, QString("Scene ")+Core::instance()->getEffectName(), QString("Channel")+QString::number(id)+" name is empty.");
            return false;

        }
        child->setAttribute("id", name->text());
        child->setAttribute("value", value->text().mid(6));
        node->appendChild(*child);
    }
    else if( !value->text().isEmpty() )
    {
        child->setAttribute("type", "image");
        if( name->text().isEmpty() )
        {
            QMessageBox::warning(NULL, QString("Scene ")+Core::instance()->getEffectName(), QString("Channel")+QString::number(id)+" name is empty.");
            return false;

        }
        child->setAttribute("id", name->text());
        child->setAttribute("value", value->text());
        node->appendChild(*child);
    }

    return true;
}

void NewShader::ok()
{
    //Modify the config file..
    QString path("./data/scenes/"+Core::instance()->getEffectName()+"/config.xml");


    //Parsing XML document.
    QDomDocument dom("config");
    QFile xml_doc(path);
    if(!xml_doc.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL, QString("Read ")+Core::instance()->getEffectName()+QString(" config"), QString("Can't open the file config.xml of ")+Core::instance()->getEffectName());
        return;
    }

    if(!dom.setContent(xml_doc.readAll()))
    {
        xml_doc.close();
        QMessageBox::warning(NULL, QString("Read ")+Core::instance()->getEffectName()+QString(" config"), "Le document XML n'a pas pu être attribué à l'objet QDomDocument.");
        return;
    }
    xml_doc.close();

    //Make the new node
    QDomElement newLayer = dom.createElement(QString("layer"));
    newLayer.setAttribute("id", ui->ID->value());
    newLayer.setAttribute("value", ui->nameLineEdit->text()+".glsl");
    if( ui->resolutionCheck->isChecked() )
    {
        newLayer.setAttribute("width", ui->width->value());
        newLayer.setAttribute("height", ui->height->value());
    }
    newLayer.setAttribute("format", ui->type->currentText());
    newLayer.setAttribute("mode", ui->display->currentText());
    if( ui->backBuffer->isChecked() )
        newLayer.setAttribute("backbuffer", "true");

    if( !writeDomChannel( ui->chan1Name, ui->chan1LineEdit, &newLayer, dom, 1) )
    {
        xml_doc.close();
        return;
    }
    if( !writeDomChannel( ui->chan2Name, ui->chan2LineEdit, &newLayer, dom, 2) )
    {
        xml_doc.close();
        return;
    }
    if( !writeDomChannel( ui->chan3Name, ui->chan3LineEdit, &newLayer, dom, 3) )
    {
        xml_doc.close();
        return;
    }
    if( !writeDomChannel( ui->chan4Name, ui->chan4LineEdit, &newLayer, dom, 4) )
    {
        xml_doc.close();
        return;
    }

    //We find where we write the new layer..
    QDomNode node = dom.documentElement().firstChild();
    while(!node.isNull())
    {
        QDomElement element = node.toElement();
        if(element.tagName() == "layer")
        {
            int id = element.attribute("id", "0").toInt();
            if( id == ui->ID->value() )
            {
                QMessageBox::warning(NULL, QString("Scene ")+Core::instance()->getEffectName(), QString("The layer with id ")+QString::number(id)+" already exist.");
                xml_doc.close();
                return;
            }
            else if( node.nextSibling().toElement().attribute("id","0").toInt() > ui->ID->value() )
            {
                break;
            }
        }
        node = node.nextSibling();
    }
    dom.documentElement().insertAfter(newLayer,node);




    QFile file("./data/scenes/test/config.xml");
    file.setPermissions(QFile::WriteUser);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text ))
    {
        QMessageBox::warning(NULL, QString("Write ")+Core::instance()->getEffectName()+QString(" config"), QString("Can't open the file config.xml of ")+Core::instance()->getEffectName());
        return;
    }
    QTextStream stream;
    stream.setDevice(&file);
    dom.save(stream, 4);
    file.close();


    //Add new file
    QFile::copy(":res/framework/Layer/layer.glsl", "./data/scenes/"+Core::instance()->getEffectName()+"/"+ui->nameLineEdit->text()+".glsl");


    emit reload();
    hide();
}

void NewShader::cancel()
{
    hide();
}


void NewShader::setPassChan1()
{
    int id = QInputDialog::getInt(this, "Layer","Enter the layer id", 0, 0, 4096, 1);
    ui->chan1LineEdit->setText( QString("Layer ") + QString::number(id) );
}

void NewShader::setPassChan2()
{
    int id = QInputDialog::getInt(this, "Layer","Enter the layer id", 0, 0, 4096, 1);
    ui->chan2LineEdit->setText( QString("Layer ") + QString::number(id) );
}

void NewShader::setPassChan3()
{
    int id = QInputDialog::getInt(this, "Layer","Enter the layer id", 0, 0, 4096, 1);
    ui->chan3LineEdit->setText( QString("Layer ") + QString::number(id) );
}

void NewShader::setPassChan4()
{
    int id = QInputDialog::getInt(this, "Layer","Enter the layer id", 0, 0, 4096, 1);
    ui->chan4LineEdit->setText( QString("Layer ") + QString::number(id) );
}

void NewShader::setImageChan1()
{
    QString path = QFileDialog::getOpenFileName(this, "Open an image", QString("data/textures"), "Images (*.png *.gif *.jpg *.jpeg *.bmp)");
    ui->chan1LineEdit->setText(QFileInfo(path).fileName());
}

void NewShader::setImageChan2()
{
    QString path = QFileDialog::getOpenFileName(this, "Open an image", QString("data/textures"), "Images (*.png *.gif *.jpg *.jpeg *.bmp)");
    ui->chan2LineEdit->setText(QFileInfo(path).fileName());
}

void NewShader::setImageChan3()
{
    QString path = QFileDialog::getOpenFileName(this, "Open an image", QString("data/textures"), "Images (*.png *.gif *.jpg *.jpeg *.bmp)");
    ui->chan3LineEdit->setText(QFileInfo(path).fileName());
}

void NewShader::setImageChan4()
{
    QString path = QFileDialog::getOpenFileName(this, "Open an image", QString("data/textures"), "Images (*.png *.gif *.jpg *.jpeg *.bmp)");
    ui->chan4LineEdit->setText(QFileInfo(path).fileName());
}
