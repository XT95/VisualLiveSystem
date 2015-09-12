#include "fbo.h"



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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);


    //Make buffer
    glGenFramebuffers(1, &m_FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_FrameBuffer);


    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_ColorTextureID, 0);
 

    glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

}


//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
FBO::~FBO()
{
    glDeleteTextures(1, &m_ColorTextureID);
    glDeleteFramebuffers(1, &m_FrameBuffer);
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
    glBindTexture(GL_TEXTURE_2D, 0);


    glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_FrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_ColorTextureID, 0);
    glBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);



    glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

}

//-----------------------------------------------------------------------------
// bind
//-----------------------------------------------------------------------------
void FBO::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_ColorTextureID);
}
