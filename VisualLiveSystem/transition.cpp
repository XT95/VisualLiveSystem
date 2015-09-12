#include <QTextStream>
#include <QtXml>
#include <QMessageBox>
#include <iostream>
#include "core.h"
#include "fast2dquad.h"
#include "transition.h"

Transition::Transition()
{
    m_shader = NULL;
    m_backBuffer = NULL;
    m_time.restart();

    m_totalTime = 1.f;
    m_switchTime = 0.f;
    m_switched = false;
}

Transition::~Transition()
{
    if(m_backBuffer)
        delete m_backBuffer;
    if(m_shader)
        delete m_shader;
}

void Transition::read(const char *filename)
{
    QString path = QString("./data/transitions/")+QString(filename)+QString("/");

    //Parsing XML document.
    QDomDocument dom("config");
    QFile xml_doc(path+"config.xml");
    if(!xml_doc.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL, QString("Error"), QString("Can't open the file config.xml of ")+filename);
        return;
    }
    if (!dom.setContent(&xml_doc))
    {
        xml_doc.close();
        QMessageBox::warning(NULL, QString("Error"), QString("Can't parse : ")+filename);
        return;
    }
    QDomNode node = dom.documentElement().firstChild();
    while(!node.isNull())
    {
        QDomElement element = node.toElement();

        if(element.tagName() == "time")
        {
            m_totalTime = element.attribute("total", "1").toFloat();
            m_switchTime = element.attribute("switch", "0").toFloat();
        }
        else if(element.tagName() == "pass")
        {
            //Compute shader
            QString filePath = element.attribute("value", "none");
            int ok;
            if(filePath != "none")
            {
                QFile file(path+filePath);
                QString strings;
                if (file.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    QTextStream in(&file);
                    while (!in.atEnd()) {
                        strings += in.readLine()+"\n";
                    }
                }
                m_shader = new Shader();
                ok = m_shader->compil(Core::instance()->getVertexShader(),strings.toStdString().c_str());
            }


            //Generate buffers !
            if(ok == SHADER_SUCCESS)
            {
                if(element.attribute("backbuffer", "false") == "true")
                {
                    m_backBuffer = new FBO();
                    m_backBuffer->setSize(Core::instance()->getResX(), Core::instance()->getResY());
                }
            }
            else
            {
                return;
                delete m_shader;
            }
        }
        node = node.nextSibling();
    }
}

void Transition::start()
{
    m_time.restart();
    m_switched = false;
}

void Transition::draw()
{
    float time = float(m_time.elapsed())/1000.f;

    glEnable(GL_TEXTURE_1D);
    if(time < m_totalTime)
    {
        m_shader->enable();
            if(time < m_switchTime)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Core::instance()->getFBOChan(Core::instance()->getCurrentChannel())->getColor());
                m_shader->sendi("first",0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, Core::instance()->getFBOChan(!Core::instance()->getCurrentChannel())->getColor());
                m_shader->sendi("second",1);
            }
            else
            {
                if(!m_switched)
                {
                    m_switched = true;
                    Core::instance()->Switch();
                }
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Core::instance()->getFBOChan(!Core::instance()->getCurrentChannel())->getColor());
                m_shader->sendi("first",0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, Core::instance()->getFBOChan(Core::instance()->getCurrentChannel())->getColor());
                m_shader->sendi("second",1);
            }

            glActiveTexture(GL_TEXTURE2);
            Core::instance()->bindTextureSpectrum();
            m_shader->sendi("spectrum",2);

            m_shader->sendf("bass",Core::instance()->getBass());
            m_shader->sendf("bassTime",Core::instance()->getBassTime());
            m_shader->sendf("time",time);


            Fast2DQuadDraw();

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_1D, 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);

        m_shader->disable();
    }
    else
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Core::instance()->getFBOChan(Core::instance()->getCurrentChannel())->getColor());
        Fast2DQuadDraw();
    }
}
