
#include <GL/glew.h>
#include <cstdlib> //fix the NULL undeclared


#define BUFFER_OFFSET(a) ((char*)NULL + (a))
struct vec2
{
    float x,y;
};

static GLuint vbo;

void Fast2DQuadInit(void)
{
	vec2 point[4];
	point[0].x =-1.0f; point[1].x = 1.0f; point[2].x =-1.0f; point[3].x = 1.0f; 
	point[0].y =-1.0f; point[1].y =-1.0f; point[2].y = 1.0f; point[3].y = 1.0f; 
	vec2 uv[4];
	uv[0].x = 0.0f; uv[1].x = 1.0f; uv[2].x = 0.0f; uv[3].x = 1.0f; 
	uv[0].y = 0.0f; uv[1].y = 0.0f; uv[2].y = 1.0f; uv[3].y = 1.0f; 

	
	//make the vertex buffer object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 4*2*sizeof(float)*2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4*2*sizeof(float), point);
	glBufferSubData(GL_ARRAY_BUFFER, 4*2*sizeof(float), 4*2*sizeof(float), uv);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}


void Fast2DQuadDraw(void)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));
	glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(4*2*sizeof(float)));
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Fast2DQuadFree(void)
{
	glDeleteBuffers(1,&vbo);
}
