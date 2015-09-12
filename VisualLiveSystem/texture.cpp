#include <GL/glew.h>
#include <QImage>
#include <QGLWidget>
#include <QMessageBox>
#include <iostream>
#include <QTextStream>
#include <QFile>
#include "texture.h"

Texture::Texture()
{
    m_id=-1;
}
Texture::~Texture()
{
    glDeleteTextures( 1, &m_id );
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::load(const std::string &path)
{
    if(m_id != -1)
        glDeleteTextures( 1, &m_id );

    QImage t;
    QImage b;

    if( !b.load( path.c_str() ) )
    {
        QMessageBox::critical(NULL, "Error", QString(QString("Can't load image ")+path.c_str()) );
    }
    t = QGLWidget::convertToGLFormat( b );


    glGenTextures( 1, &m_id );
    glBindTexture( GL_TEXTURE_2D, m_id );
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );
}
