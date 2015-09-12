#include <QTextStream>
#include <QtXml>
#include <QMessageBox>
#include <iostream>
#include "core.h"
#include "fast2dquad.h"
#include "scene.h"

Scene::Scene()
{
    m_nbLayer = 0;
    m_nbCopy = 0;
    m_nbParticles = 0;
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
    delete[] m_copy;
    glDeleteBuffers(1,&m_vboParticle);
}


static unsigned int mirand = 1;
float sfrand(void);
void Scene::generateParticles()
{
    float *vertex = new float[m_nbParticles*3];
    float *uv = new float[m_nbParticles*2];


    {
    int size = sqrt(m_nbParticles);
    int id = 0;
    for(int y=0; y<size; y++ )
    for(int x=0; x<size; x++ )
    {
        vertex[ id+0] = 0.;
        vertex[ id+1] = 0.;
        vertex[ id+2] = 0.;
        id += 3;
    }
    }
    int size = sqrt(m_nbParticles);
    std::cout << size << std::endl;
    int id = 0;
    for(int y=0; y<size; y++ )
    for(int x=0; x<size; x++ )
    {
        uv[ id+0] = (float(x))/float(size); // u
        uv[ id+1] = (float(y))/float(size); // v
        id += 2;
    }


    //make the vertex buffer object
    glGenBuffers(1, &m_vboParticle);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboParticle);
    glBufferData(GL_ARRAY_BUFFER, m_nbParticles*sizeof(float)*3*2, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_nbParticles*3*sizeof(float), vertex);
    glBufferSubData(GL_ARRAY_BUFFER, m_nbParticles*3*sizeof(float), m_nbParticles*2*sizeof(float), uv);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    delete[] vertex;
    delete[] uv;
}

void Scene::drawParticles()
{
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    /*glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_ALPHA_TEST);*/


    glBindBuffer(GL_ARRAY_BUFFER, m_vboParticle);
    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(m_nbParticles*3*sizeof(float)));
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glDrawArrays(GL_POINTS, 0, m_nbParticles);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);
    /*glDisable(GL_ALPHA_TEST);
    glDisable(GL_DEPTH_TEST);*/

    glDisable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_POINT_SPRITE);
}


void Scene::read(const char *filename)
{
    QString path;
    if(!filename)
        path = QString("./data/scenes/open/");
    else
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

    //Count layers
    QDomNode node = dom.documentElement().firstChild();
    while(!node.isNull())
    {
        QDomElement element = node.toElement();
        if(element.tagName() == "layer")
            m_nbLayer++;
        if(element.tagName() == "copy")
            m_nbCopy++;
        node = node.nextSibling();
    }

    //Init new layers & copyers
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
    m_copy = new Copy[m_nbCopy];
    for(int i=0; i<m_nbCopy; i++)
    {
        m_copy[i].src = NULL;
        m_copy[i].dest = NULL;
    }

    //Read the xml file
    node = dom.documentElement().firstChild();
    int textureID=0;
    while(!node.isNull())
    {
        QDomElement element = node.toElement();

        if(element.tagName() == "copy")
        {
            int id = element.attribute("id", "0").toInt();
            int src = element.attribute("src", "0").toInt();
            int dest = element.attribute("dest", "0").toInt();
            m_copy[id].src = m_layer[src].buffer;
            m_copy[id].dest = m_layer[dest].backBuffer;
        }
        else if(element.tagName() == "layer")
        {
            //Get infos ..
            int id = element.attribute("id", "0").toInt();
            QString format = element.attribute("format", "rgba");

            QString blend = element.attribute("mode", "default");
            m_layer[id].width = element.attribute("width", "-1").toInt();
            m_layer[id].height = element.attribute("height", "-1").toInt();

            QString type = element.attribute("type", "pixel");
            if(type == "pixel")
                m_layer[id].type = PIXEL;
            else if(type == "particle")
            {
                m_layer[id].type = PARTICLE;
                m_nbParticles = element.attribute("count", "0").toInt();
                generateParticles();
            }

            m_layer[id].iterate = element.attribute("iterate", "0").toInt();

            if(m_layer[id].width == -1)
                m_layer[id].width = Core::instance()->getResX();
            if(m_layer[id].height == -1)
                m_layer[id].height = Core::instance()->getResY();

            if(blend != "default")
            {
                m_layer[id].tmpBuffer = new FBO();
                m_layer[id].tmpBuffer->setSize(m_layer[id].width, m_layer[id].height);
            }
            //Select format
            GLint bufferFormat = GL_RGBA;
            if(format == "rgb")
                bufferFormat = GL_RGB;
            else if(format == "rgba")
                bufferFormat = GL_RGBA;
            else if(format == "rgba32")
                bufferFormat = GL_RGBA32F_ARB; //work ?
            else if(format == "rgba32")
                bufferFormat = GL_RGBA32F_ARB; //work ?
            else if(format == "rgba32half")
                bufferFormat = GL_RGBA16F_ARB; //work ?
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
            else if(blend == "none")
                m_layer[id].mode = NONE;
            else
                QMessageBox::warning(NULL, QString("Scene ")+filename, QString("This layer mode doesn't exist : ")+blend);

            //Compute shader
            QString filePath = element.attribute("value", "none");
            m_layer[id].name = filePath;
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
                Core::instance()->writeInfo( QString("Compiling ") + m_layer[id].name + QString(".. ") );
                m_layer[id].shader = new Shader();
                if( m_layer[id].type == PIXEL)
                    ok = m_layer[id].shader->compil(Core::instance()->getVertexShader(),strings.toStdString().c_str());
                else if( m_layer[id].type == PARTICLE)
                    ok = m_layer[id].shader->compil(Core::instance()->getVertexShaderParticle(),strings.toStdString().c_str());

                if(ok == SHADER_SUCCESS)
                    Core::instance()->writeInfo( QString("[Done]\n"), false );
                file.close();
            }


            //Generate buffers !
            if(ok == SHADER_SUCCESS)
            {
                m_layer[id].buffer = new FBO();
                m_layer[id].buffer->setSize(m_layer[id].width, m_layer[id].height);
                m_layer[id].buffer->setFormat(bufferFormat);
                if(element.attribute("backbuffer", "false") == "true" || m_layer[id].iterate > 0)
                {
                    m_layer[id].backBuffer = new FBO();
                    m_layer[id].backBuffer->setSize(m_layer[id].width, m_layer[id].height);
                    m_layer[id].backBuffer->setFormat(bufferFormat);
                }
            }
            else
            {
                return;
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

void Scene::compilShader(const char *txt, int id)
{
    Core::instance()->writeInfo( QString("Compiling ") + m_layer[id].name + QString(".. ") );
    int ok;
    if( m_layer[id].type == PIXEL)
        ok = m_layer[id].shader->compil(Core::instance()->getVertexShader(),txt);
    else if( m_layer[id].type == PARTICLE)
        ok = m_layer[id].shader->compil(Core::instance()->getVertexShaderParticle(),txt);
    if(ok == SHADER_SUCCESS)
        Core::instance()->writeInfo( QString("[Done]\n"), false );
}

void Scene::resetTime()
{
    m_time = 0.f;
}

void Scene::compute()
{
    m_time += float(m_QTime.elapsed())*0.001f;
    m_QTime.restart();

    for(int i=0; i<m_nbLayer; i++)
    {
        //Select buffer
        FBO *buffer[2];
        if( m_layer[i].mode == DEFAULT )
        {
            buffer[0] = m_layer[i].buffer;
            buffer[1] = m_layer[i].backBuffer;
        }
        else
        {
            buffer[0] = m_layer[i].tmpBuffer;
            buffer[1] = m_layer[i].backBuffer;
        }

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

        for(int j=0; j<m_layer[i].iterate+1; j++)
        {
            //Enable fbo
            buffer[j%2]->enable();

            if(m_layer[i].backBuffer)
            {
                glActiveTexture(GL_TEXTURE5);
                m_layer[i].shader->sendi("backBuffer",5);
                glBindTexture(GL_TEXTURE_2D, buffer[!(j%2)]->getColor());
            }


            m_layer[i].shader->sendf("bass",0.f);
            m_layer[i].shader->sendf("bassTime",0.f);
            m_layer[i].shader->sendf("time",m_time);
            m_layer[i].shader->sendf("CC[0]", Core::instance()->getCC(0));
            m_layer[i].shader->sendf("CC[1]", Core::instance()->getCC(1));
            m_layer[i].shader->sendf("CC[2]", Core::instance()->getCC(2));
            m_layer[i].shader->sendf("CC[3]", Core::instance()->getCC(3));
            m_layer[i].shader->sendi("iteration", j);


            glEnable(GL_TEXTURE_3D);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_3D, Core::instance()->getNoise3D());
            m_layer[i].shader->sendi("noise3D",0);

            if(m_layer[i].type == PIXEL)
                Fast2DQuadDraw();
            else if(m_layer[i].type == PARTICLE)
            {
                glClear(GL_COLOR_BUFFER_BIT);
                drawParticles();
            }

            glBindTexture(GL_TEXTURE_3D, 0);
            glDisable(GL_TEXTURE_3D);


            for(int j=5; j<=0; j--)
            {
                glActiveTexture(GL_TEXTURE0+j);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            buffer[j%2]->disable();
        }
        m_layer[i].shader->disable();

        if(m_layer[i].mode != DEFAULT)
        {
            glMatrixMode(GL_PROJECTION);
                    glPushMatrix();
                    glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
                    glPushMatrix();
                    glLoadIdentity();
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
            glMatrixMode(GL_PROJECTION);
                    glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
                glPopMatrix();

        }



        //Compute back buffer if needed..
        if(m_layer[i].backBuffer)
        {
            glMatrixMode(GL_PROJECTION);
                    glPushMatrix();
                    glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
                    glPushMatrix();
                    glLoadIdentity();
            m_layer[i].backBuffer->enable();
                glActiveTexture(GL_TEXTURE0);
                if(m_layer[i].mode == DEFAULT)
                    glBindTexture(GL_TEXTURE_2D, m_layer[i].buffer->getColor());
                else
                    glBindTexture(GL_TEXTURE_2D, m_layer[i].tmpBuffer->getColor());
                Fast2DQuadDraw();
            m_layer[i].backBuffer->disable();
            glMatrixMode(GL_PROJECTION);
                    glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
                glPopMatrix();
        }

    }

    //Copy
    glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();
    for(int i=0; i<m_nbCopy; i++)
    {
        m_copy[i].dest->enable();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_copy[i].src->getColor());
            Fast2DQuadDraw();
        m_copy[i].dest->disable();

    }
    glMatrixMode(GL_PROJECTION);
            glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
}

void Scene::draw()
{
    if(m_nbLayer>0)
    {
        glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glLoadIdentity();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_layer[m_nbLayer-1].buffer->getColor());
        Fast2DQuadDraw();

        glMatrixMode(GL_PROJECTION);
                glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
            glPopMatrix();

    }
}
