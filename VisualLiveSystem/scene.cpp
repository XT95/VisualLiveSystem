#include <QTextStream>
#include <QtXml>
#include <QMessageBox>
#include <iostream>
#include "core.h"
#include "fast2dquad.h"
#include "scene.h"

Scene::Scene()
{
    m_id = 0;
    m_nbLayer = 0;
    m_layer = NULL;
    m_time = 0.f;
    m_param[0] = QString("0");
    m_param[1] = QString("1");
    m_param[2] = QString("2");
    m_param[3] = QString("3");
    m_QTime.restart();
}

Scene::~Scene()
{
    for(int i=0; i<m_nbLayer; i++)
    {
        if(m_layer[i].buffer)
            delete m_layer[i].buffer;
        if(m_layer[i].backBuffer)
            delete m_layer[i].backBuffer;
        if(m_layer[i].shader)
            delete m_layer[i].shader;
        for(int j=0; j<4; j++)
            if(m_layer[i].channel[j])
                delete m_layer[i].channel[j];
    }
}


void Scene::read(const char *filename)
{
    QString path;
    if(!filename)
        path = QString("./data/scenes/open/");
    else
        path = QString("./data/scenes/")+QString(filename)+QString("/");

    //Parsing XML document.
    QDomDocument dom("config");
    QFile xml_doc(path+"config.xml");
    if(!xml_doc.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL, QString("Read ")+filename+QString(" config"), QString("Can't open the file config.xml of ")+filename);
        return;
    }
    if (!dom.setContent(&xml_doc))
    {
        xml_doc.close();
        QMessageBox::warning(NULL, QString("Read ")+filename+QString(" config"), "Le document XML n'a pas pu être attribué à l'objet QDomDocument.");
        return;
    }

    //Count layers
    QDomNode node = dom.documentElement().firstChild();
    while(!node.isNull())
    {
        QDomElement element = node.toElement();
        if(element.tagName() == "layer")
            m_nbLayer++;
        node = node.nextSibling();
    }

    //Init new layers
    m_layer = new Layer[m_nbLayer];
    for(int i=0; i<m_nbLayer; i++)
    {
        m_layer[i].shader = NULL;
        m_layer[i].buffer = NULL;
        m_layer[i].backBuffer = NULL;
        m_layer[i].tmpBuffer = NULL;
        m_layer[i].mode = DEFAULT;
        for(int j=0; j<4; j++)
        {
            m_layer[i].channel[j] = NULL;
        }
    }

    //Read the xml file
    node = dom.documentElement().firstChild();
    int textureID=0;
    while(!node.isNull())
    {
        QDomElement element = node.toElement();
        if(element.tagName() == "layer")
        {
            //Get infos ..
            int id = element.attribute("id", "0").toInt();
            QString format = element.attribute("format", "rgba");

            QString blend = element.attribute("mode", "default");
            m_layer[id].width = element.attribute("width", "-1").toInt();
            m_layer[id].height = element.attribute("height", "-1").toInt();

            if(m_layer[id].width == -1)
                m_layer[id].width = Core::instance()->getResX();
            if(m_layer[id].height == -1)
                m_layer[id].height = Core::instance()->getResY();

            if(blend != "default")
            {
                m_layer[id].tmpBuffer = new FBO();
                m_layer[id].tmpBuffer->setSize(m_layer[id].width, m_layer[id].height);
                m_layer[id].width = Core::instance()->getResX();
                m_layer[id].height = Core::instance()->getResY();
            }

            //Select format
            GLint bufferFormat = GL_RGBA;
            if(format == "rgb")
                bufferFormat = GL_RGB;
            else if(format == "rgba")
                bufferFormat = GL_RGBA;
            else if(format == "rgba32")
                bufferFormat = GL_RGBA32F_ARB; //TODOOOOOOO
            else if(format == "luminance")
                bufferFormat = GL_LUMINANCE;
            else
                QMessageBox::warning(NULL, QString("Scene ")+filename, QString("This layer format doesn't exist : ")+format);

            //Select mode
            if(blend == "default")
                m_layer[id].mode = DEFAULT;
            else if(blend == "alpha")
                m_layer[id].mode = BLEND_ALPHA;
            else if(blend == "add")
                m_layer[id].mode = BLEND_ADD;
            else
                QMessageBox::warning(NULL, QString("Scene ")+filename, QString("This layer mode doesn't exist : ")+blend);

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
                m_layer[id].shader = new Shader();
                ok = m_layer[id].shader->compil(Core::instance()->getVertexShader(),strings.toStdString().c_str());
            }


            //Generate buffers !
            if(ok == SHADER_SUCCESS)
            {
                m_layer[id].buffer = new FBO();
                m_layer[id].buffer->setSize(m_layer[id].width, m_layer[id].height);
                m_layer[id].buffer->setFormat(bufferFormat);
                if(element.attribute("backbuffer", "false") == "true")
                {
                    m_layer[id].backBuffer = new FBO();
                    m_layer[id].backBuffer->setSize(m_layer[id].width, m_layer[id].height);
                    m_layer[id].backBuffer->setFormat(bufferFormat);
                }
            }
            else
            {
                return;
                delete m_layer[id].shader;
            }

            //Load channel
            textureID=0;
            QDomNode n = element.firstChild();
            while(!n.isNull())
            {
                QDomElement e = n.toElement();
                if(e.tagName() == "channel")
                {
                    if(textureID<4)
                    {
                        m_layer[id].channelName[textureID] = e.attribute("id", "tex");
                        if( e.attribute("type", "") == "image" )
                        {
                            m_layer[id].channel[textureID] = new Texture();
                            m_layer[id].channel[textureID]->load("./data/textures/"+e.attribute("value", "none").toStdString());
                        }
                        else if( e.attribute("type", "") == "layer" )
                        {
                            m_layer[id].channel[textureID] = m_layer[e.attribute("value", "0").toInt()].buffer;
                            m_layer[e.attribute("value", "0").toInt()].buffer->bind();
                        }
                        textureID++;
                    }
                    else
                        QMessageBox::warning(NULL, QString("Read ")+filename+QString(" config"), QString("Too many textures (max is 4)"));
                }
                n = n.nextSibling();
            }
        }
        else if(element.tagName() == "param")
        {
            m_param[element.attribute("id", "0").toInt()] = element.attribute("value","none");
        }
        node = node.nextSibling();
    }
}

void Scene::resetTime()
{
    m_time = 0.f;
}

void Scene::compute()
{
    m_time += float(m_QTime.elapsed())*0.001f*Core::instance()->getTimeSpeed();
    m_QTime.restart();

    for(int i=0; i<m_nbLayer; i++)
    {
        //Enable fbo
        if(m_layer[i].mode == DEFAULT)
            m_layer[i].buffer->enable();
        else
            m_layer[i].tmpBuffer->enable();

            m_layer[i].shader->enable();
            for(int j=0; j<4; j++)
            {
                if(m_layer[i].channel[j] != NULL)
                {
                    glActiveTexture(GL_TEXTURE0+j);
                    m_layer[i].channel[j]->bind();
                    m_layer[i].shader->sendi(m_layer[i].channelName[j].toStdString().c_str(), j);
                }
            }
                if(i>0)
                {
                    glActiveTexture(GL_TEXTURE4);
                    m_layer[i].shader->sendi("lastBuffer",4);
                    glBindTexture(GL_TEXTURE_2D, m_layer[i-1].buffer->getColor());
                }
                if(m_layer[i].backBuffer)
                {
                    glActiveTexture(GL_TEXTURE5);
                    m_layer[i].shader->sendi("backBuffer",5);
                    glBindTexture(GL_TEXTURE_2D, m_layer[i].backBuffer->getColor());
                }

                glActiveTexture(GL_TEXTURE6);
                Core::instance()->bindTextureSpectrum();
                m_layer[i].shader->sendi("spectrum",6);

                m_layer[i].shader->sendf("bass",Core::instance()->getBass());
                m_layer[i].shader->sendf("noise_lvl",Core::instance()->getNoiseLvl());
                m_layer[i].shader->sendf("bassTime",Core::instance()->getBassTime());
                m_layer[i].shader->sendf("time",m_time);
                m_layer[i].shader->sendf("CC[0]", Core::instance()->getCC(m_id,0));
                m_layer[i].shader->sendf("CC[1]", Core::instance()->getCC(m_id,1));
                m_layer[i].shader->sendf("CC[2]", Core::instance()->getCC(m_id,2));
                m_layer[i].shader->sendf("CC[3]", Core::instance()->getCC(m_id,3));


                glEnable(GL_TEXTURE_3D);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_3D, Core::instance()->getNoise3D());
                m_layer[i].shader->sendi("noise3D",0);
                Fast2DQuadDraw();
                glBindTexture(GL_TEXTURE_3D, 0);
                glDisable(GL_TEXTURE_3D);



                glActiveTexture(GL_TEXTURE6);
                glBindTexture(GL_TEXTURE_1D, 0);
                for(int j=5; j<=0; j--)
                {
                    glActiveTexture(GL_TEXTURE0+j);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    if(j<4)
                    {
                        /*if(m_texture[j].getName() == "noise3D")
                        {
                        }*/
                    }
                }

            m_layer[i].shader->disable();

        if(m_layer[i].mode == DEFAULT)
            m_layer[i].buffer->disable();
        else
        {
            m_layer[i].tmpBuffer->disable();

            m_layer[i].buffer->enable();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_layer[(int)fmax(i-1,0)].buffer->getColor());
                Fast2DQuadDraw();

                glEnable(GL_BLEND);
                if(m_layer[i].mode == BLEND_ALPHA)
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                else //if(m_layer[i].mode == BLEND_ADD)
                    glBlendFunc(GL_ONE, GL_ONE);
                glBindTexture(GL_TEXTURE_2D, m_layer[i].tmpBuffer->getColor());
                Fast2DQuadDraw();
                glDisable(GL_BLEND);

            m_layer[i].buffer->disable();

        }



        //Compute back buffer if needed..
        if(m_layer[i].backBuffer)
        {
            m_layer[i].backBuffer->enable();
                glActiveTexture(GL_TEXTURE0);
                if(m_layer[i].mode == DEFAULT)
                    glBindTexture(GL_TEXTURE_2D, m_layer[i].buffer->getColor());
                else
                    glBindTexture(GL_TEXTURE_2D, m_layer[i].tmpBuffer->getColor());
                Fast2DQuadDraw();
            m_layer[i].backBuffer->disable();
        }

    }
    glDisable(GL_TEXTURE_1D);
}

void Scene::draw()
{
    if(m_nbLayer>0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_layer[m_nbLayer-1].buffer->getColor());
        Fast2DQuadDraw();
    }
}

void Scene::setPreview(bool t)
{
    if(t)
    {
        for(int i=0; i<m_nbLayer; i++)
        {
            if(m_layer[i].buffer)
                m_layer[i].buffer->setSize( m_layer[i].width/8, m_layer[i].height/8);
            if(m_layer[i].backBuffer)
                m_layer[i].backBuffer->setSize( m_layer[i].width/8, m_layer[i].height/8);
            if(m_layer[i].tmpBuffer)
                m_layer[i].tmpBuffer->setSize( m_layer[i].width/8, m_layer[i].height/8);
        }
    }
    else
    {
        for(int i=0; i<m_nbLayer; i++)
        {
            if(m_layer[i].buffer)
                m_layer[i].buffer->setSize( m_layer[i].width, m_layer[i].height);
            if(m_layer[i].backBuffer)
                m_layer[i].backBuffer->setSize( m_layer[i].width, m_layer[i].height);
            if(m_layer[i].tmpBuffer)
                m_layer[i].tmpBuffer->setSize( m_layer[i].width, m_layer[i].height);
        }
    }
    glFlush();
    glFinish();
}
