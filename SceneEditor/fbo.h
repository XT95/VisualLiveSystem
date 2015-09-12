#ifndef DEF_CORE_FBO_H
#define DEF_CORE_FBO_H

#include <GL/glew.h>

#include "texture.h"

//-----------------------------------------------------------------------------
// Class Frame Buffer Object
//-----------------------------------------------------------------------------
class FBO : public Texture
{
public:
       FBO();
       ~FBO();

       void enable(void);
       void disable(void);


       void setSize( int width, int height );
       void setFormat(GLint format);

       int  getSizeX(){return m_width;}
       int  getSizeY(){return m_height;}
       GLuint getColor(void);
       GLenum getFormat(){return m_format;}

       void bind();
private:
       void updateTexture();

       GLuint m_FrameBuffer;
       GLuint m_DepthRenderBuffer;
       GLuint m_ColorTextureID;
       GLuint m_DepthTextureID;
       int    m_width,m_height;
       GLint  m_format;
};


#endif
