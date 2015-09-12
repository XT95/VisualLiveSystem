#include "fbo.h"

#include <iostream>


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
FBO::FBO()
{
    m_FrameBuffer=0;
    m_ColorTextureID=0;
    m_width = 1280;
    m_height = 720;
    m_format = GL_RGBA;

    //Create texture.
    glGenTextures(1, &m_ColorTextureID);
    glBindTexture(GL_TEXTURE_2D, m_ColorTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);


    glGenTextures(1, &m_DepthTextureID);
    glBindTexture(GL_TEXTURE_2D, m_DepthTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE,   GL_LUMINANCE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,   GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,   GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);


    //Make buffer
    glGenFramebuffers(1, &m_FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_FrameBuffer);



    glGenRenderbuffers(1, &m_DepthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER_EXT, m_DepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, m_width, m_height);
    glBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_ColorTextureID, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT, m_DepthRenderBuffer);


    glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

}


//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
FBO::~FBO()
{
    glDeleteTextures(1, &m_ColorTextureID);
    glDeleteTextures(1, &m_DepthTextureID);
    glDeleteFramebuffers(1, &m_FrameBuffer);
    glDeleteRenderbuffers(1, &m_DepthRenderBuffer);
}


//-----------------------------------------------------------------------------
// Enable
//-----------------------------------------------------------------------------
void FBO::enable(void)
{
    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0,0,m_width,m_height);

    glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_FrameBuffer);
    glPushMatrix();
}

//-----------------------------------------------------------------------------
// Disable
//-----------------------------------------------------------------------------
void FBO::disable(void)
{
    glPopMatrix();
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
    glPopAttrib();
}



//-----------------------------------------------------------------------------
// Set
//-----------------------------------------------------------------------------
void FBO::setSize( int width, int height )
{
    m_width = width;
    m_height = height;


    updateTexture();
}
void FBO::setFormat(GLint format)
{
    m_format = format;
    updateTexture();
}


//-----------------------------------------------------------------------------
// Get
//-----------------------------------------------------------------------------
GLuint FBO::getColor(void)
{
    return m_ColorTextureID;
}

//-----------------------------------------------------------------------------
// updateTexture
//-----------------------------------------------------------------------------
void FBO::updateTexture()
{
    glBindTexture(GL_TEXTURE_2D, m_ColorTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, m_DepthTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

}

//-----------------------------------------------------------------------------
// bind
//-----------------------------------------------------------------------------
void FBO::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_ColorTextureID);
}
