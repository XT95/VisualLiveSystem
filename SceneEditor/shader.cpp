#include <iostream>
#include "shader.h"
#include "core.h"
#include <QMessageBox>



//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Shader::Shader()
{

}
//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Shader::~Shader()
{

}

//-----------------------------------------------------------------------------
// enable/disable
//-----------------------------------------------------------------------------
void Shader::enable()
{
    glUseProgramObjectARB(m_program);
}
void Shader::disable()
{
    glUseProgramObjectARB(0);
}


//-----------------------------------------------------------------------------
// send uniform
//-----------------------------------------------------------------------------
void Shader::sendi(const char *name, int x)
{
	glUniform1i(glGetUniformLocation(m_program,name), x);
}
void Shader::sendf(const char *name, float x)
{
	glUniform1f(glGetUniformLocation(m_program,name), x);
}
void Shader::sendf(const char *name, float x, float y)
{
    glUniform2f(glGetUniformLocation(m_program,name), x,y);
}
void Shader::sendf(const char *name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(m_program,name), x,y,z);
}
void Shader::sendf(const char *name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(m_program,name), x,y,z,w);
}


//-----------------------------------------------------------------------------
// compil
//-----------------------------------------------------------------------------
int Shader::compil( const char *vertex, const char *fragment )
{
    m_program = glCreateProgramObjectARB();
	if( !makeShader(vertex, GL_VERTEX_SHADER_ARB) )
    {
        //QMessageBox::critical(NULL, "Error", "Can't compil the vertex shader !");
		return SHADER_VERTEX_ERROR;
    }
    if( !makeShader(fragment, GL_FRAGMENT_SHADER_ARB) )
    {
        //QMessageBox::critical(NULL, "Error", "Can't compil this pixel shader !");
        return SHADER_FRAGMENT_ERROR;
    }

    glLinkProgram(m_program);

	return SHADER_SUCCESS;
}


bool Shader::makeShader(const char *txt, GLuint type)
{
    GLuint object = glCreateShaderObjectARB(type);
	glShaderSource(object, 1, (const GLchar**)(&txt), NULL);
	glCompileShader(object);
	
    if( checkShader(object) )
    {
        glAttachObjectARB(m_program, object);
        glDeleteObjectARB(object);
    }
    else
    {
        return false;
    }
    return true;
}

bool Shader::checkShader(int id)
{
    GLint ok = 0;

    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if( ok != GL_TRUE )
    {
        GLint logLength = 0;
        GLchar* log = NULL;

        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
        log = new GLchar[logLength];
        glGetShaderInfoLog(id, logLength, &logLength, log);


        Core::instance()->writeError( "\n"+QString(log)+"\n", false );

        delete[] log;

        return false;
    }
    else
    {
        GLint logLength = 0;
        GLchar* log = NULL;

        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
        log = new GLchar[logLength];
        glGetShaderInfoLog(id, logLength, &logLength, log);

        if(logLength>0)
            Core::instance()->writeWarning("\n"+QString(log)+"\n", false );

        delete[] log;
    }

    return true;
}




