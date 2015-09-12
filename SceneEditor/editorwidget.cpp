#include <QTextStream>
#include <QtXml>
#include <QMessageBox>
#include <iostream>
#include "editorwidget.h"
#include "ui_editorwidget.h"

#include "core.h"
#include "texteditor.h"

EditorWidget::EditorWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::EditorWidget)
{
    ui->setupUi(this);
}

EditorWidget::~EditorWidget()
{
    delete ui;
}


void EditorWidget::loadScene(QString filename)
{
    ui->tab->clear();

    QString path;
    path = "./data/scenes/"+QString(filename)+"/";

    //Parsing XML document.
    QDomDocument dom("config");
    QFile xml_doc(path+"config.xml");
    if(!xml_doc.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL, QString("Read ")+filename+QString(" config"), QString("Can't open the file config.xml of ")+filename);
        return;
    }
    if (!dom.setContent(xml_doc.readAll()))
    {
        xml_doc.close();
        QMessageBox::warning(NULL, QString("Read ")+filename+QString(" config"), "Le document XML n'a pas pu être attribué à l'objet QDomDocument.");
        return;
    }
    xml_doc.close();


    TextEditor *t = new TextEditor();
    t->open(path+"config.xml",-1);
    ui->tab->addTab(t,"Config");

    //Count layers
    QDomNode node = dom.documentElement().firstChild();
    while(!node.isNull())
    {
        QDomElement element = node.toElement();
        if(element.tagName() == "layer")
        {
            QString file( element.attribute("value", "none") );
            TextEditor *t = new TextEditor();
            t->open(path+file, element.attribute("id", "-1").toInt());
            connect(t, SIGNAL(shaderUpdate(QString,int)), this, SLOT(shaderUpdate(QString,int)));
            ui->tab->addTab(t,file.mid(0,file.length()-5));
        }
        if(element.tagName() == "param")
        {
            emit nameCCChanged(element.attribute("id", "0").toInt(), element.attribute("value", "none"));
        }
        node = node.nextSibling();
    }


}

void EditorWidget::save()
{
    for(int i=0; i<ui->tab->count(); i++)
    {
        std::cout << "EditorWidget" << std::endl;
        ((TextEditor*)ui->tab->widget(i))->save();
    }
    Core::instance()->writeInfo("Effect saved.\n");
}

void EditorWidget::shaderUpdate(QString txt, int id)
{
    emit recompil(txt,id);
}
