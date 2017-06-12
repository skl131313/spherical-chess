
#pragma once

#include <algorithm>
#include <string>


class OpenGLTexture
{
public:

    static OpenGLTexture Make(GLenum target);
    static OpenGLTexture MakeFromFile(const char* filename);

    OpenGLTexture() = default;
    ~OpenGLTexture()
    {
        if(texture) glDeleteTextures(1, &texture);
    }

    OpenGLTexture(const OpenGLTexture&) = delete;
    OpenGLTexture(OpenGLTexture&& other)
    {
        textureTarget = other.textureTarget;
        std::swap(texture, other.texture);
    }

    OpenGLTexture& operator = (OpenGLTexture other)
    {
        std::swap(textureTarget, other.textureTarget);
        std::swap(texture, other.texture);

        return *this;
    }


    void Bind()
    {
        glBindTexture(textureTarget, texture);
    }

    GLenum GetTarget() const
    {
        return textureTarget;
    }

private:
    
    GLenum textureTarget;
    GLuint texture = 0;

};