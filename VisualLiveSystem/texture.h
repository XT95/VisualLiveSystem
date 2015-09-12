#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <iostream>


class Texture
{
public:
    Texture();
    ~Texture();

    void load(const std::string &path);
    virtual void bind();
private:
    GLuint m_id;
};

#endif // TEXTURE_H
