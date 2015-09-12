#ifndef DEF_SHADER
#define DEF_SHADER


#include <GL/glew.h>

#define SHADER_SUCCESS        1
#define SHADER_VERTEX_ERROR   2
#define SHADER_FRAGMENT_ERROR 3

class Shader
{
public:
	Shader();
	~Shader();
	
	int compil(const char *vertex, const char *fragment);
	
	void enable();
	void disable();
	
	void sendi(const char *name, int x);
	void sendf(const char *name, float x);
	void sendf(const char *name, float x, float y);
	void sendf(const char *name, float x, float y, float z);
	void sendf(const char *name, float x, float y, float z, float w);
private:
	bool makeShader(const char *txt, GLuint type);
	GLuint m_program;
};



#endif
