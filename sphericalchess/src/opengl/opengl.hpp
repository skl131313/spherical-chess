
#pragma once

#ifdef GLAD_DEBUG
#include "glad_debug.h"
#else
#include "glad.h"
#endif


#include <algorithm>

#include "texture.hpp"
#include "shaderprogram.hpp"


class OpenGLVertexArray
{
public:

    static OpenGLVertexArray Make()
    {
        OpenGLVertexArray result;
        glGenVertexArrays(1, &result.object);
        return result;
    }

    OpenGLVertexArray()                         = default;
    OpenGLVertexArray(const OpenGLVertexArray&) = delete;

    ~OpenGLVertexArray()
    {
        if(object) glDeleteVertexArrays(1, &object);
    }

    OpenGLVertexArray(OpenGLVertexArray&& other)
    {
        std::swap(object, other.object);
    }

    OpenGLVertexArray& operator = (OpenGLVertexArray other)
    {
        std::swap(object, other.object);
        return *this;
    }

    void Bind()
    {
        glBindVertexArray(object);
    }

private:

    GLuint object = 0;
};


class OpenGLBuffer
{
public:

    static OpenGLBuffer Make()
    {
        OpenGLBuffer result;
        glGenBuffers(1, &result.object);
        return result;
    }

    OpenGLBuffer()                    = default;
    OpenGLBuffer(const OpenGLBuffer&) = delete;

    ~OpenGLBuffer()
    {
        if(object) glDeleteBuffers(1, &object);
    }

    OpenGLBuffer(OpenGLBuffer&& other)
    {
        std::swap(object, other.object);
    }

    OpenGLBuffer& operator = (OpenGLBuffer other)
    {
        std::swap(object, other.object);
        return *this;
    }

    void Bind(GLenum target)
    {
        glBindBuffer(target, object);
    }

private:

    GLuint object = 0;
};
