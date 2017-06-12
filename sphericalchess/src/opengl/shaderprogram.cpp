
#include "shaderprogram.hpp"

#include <cassert>
#include <stdexcept>
#include <vector>

#include <iostream>

namespace
{

struct ShaderResource
{
    ShaderResource() = default;

    ~ShaderResource()
    {
        if(shader) glDeleteShader(shader);
    }

    ShaderResource(ShaderResource&& other)
    {
        std::swap(shader, other.shader);
    }

    ShaderResource(const ShaderResource&)       = delete;
    ShaderResource& operator = (ShaderResource) = delete;
    

    static ShaderResource Make(GLenum type, const char* source)
    {
        ShaderResource resource(type);

        GLint compiled;

        glShaderSource(resource.shader, 1, &source, nullptr);
        glCompileShader(resource.shader);

        glGetShaderiv(resource.shader, GL_COMPILE_STATUS, &compiled);


        GLint length = 0;
        glGetShaderiv(resource.shader, GL_INFO_LOG_LENGTH, &length);

        if(length > 1)
        {
            std::vector<char> buffer(length);

            glGetShaderInfoLog(resource.shader, length, nullptr, buffer.data());

            if(compiled)
            {
                std::cerr << buffer.data() << std::endl;
            }
            else
            {
                throw std::runtime_error(buffer.data());
            }
        }
        else if(!compiled)
        {
            throw std::runtime_error("Error compiling shader, no error information available.");
        }

        return resource;
    }

    GLuint GetShader() const { return shader; }

private:

    explicit ShaderResource(GLenum type) : shader(glCreateShader(type))
    {
    }

    GLuint shader = 0;
};

}



ShaderProgram ShaderProgram::MakeFromFiles(const char* vertexFilename, const char* fragmentFilename)
{
    auto vertexSource   = Util::ReadStringFile(vertexFilename);
    auto fragmentSource = Util::ReadStringFile(fragmentFilename);

    auto program = Program::Make();

    ShaderResource vertexShader   = ShaderResource::Make(GL_VERTEX_SHADER,   vertexSource.c_str());
    ShaderResource fragmentShader = ShaderResource::Make(GL_FRAGMENT_SHADER, fragmentSource.c_str());

    ShaderProgram result;

    auto prog = program.Value();

    glAttachShader(prog, vertexShader.GetShader());
    glAttachShader(prog, fragmentShader.GetShader());

    glLinkProgram(prog);

    GLint linked;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);

    GLint length = 0;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &length);

    if(length > 1)
    {
        std::vector<char> buffer(length);

        glGetProgramInfoLog(prog, length, nullptr, buffer.data());

        if(linked)
        {
            std::cerr << buffer.data() << std::endl;
        }
        else
        {
            throw std::runtime_error(buffer.data());
        }
    }
    else if(!linked)
    {
        throw std::runtime_error("Error linking program, no information available.");
    }


    GLint activeUniformCount = 0;

    glGetProgramiv(prog, GL_ACTIVE_UNIFORMS, &activeUniformCount);

    for(int i = 0; i < activeUniformCount; ++i)
    {
        GLchar buffer[128];
        GLsizei length = 0;

        glGetActiveUniformName(prog, i, std::size(buffer), &length, buffer);

        std::string name = std::string(buffer, length);
        
        // sections of an array can be considered "active" so we want to get the location
        // of the uniform array and not the specific index that is active, only extract the name
        // before the subscript "[x]".

        auto it = name.find_first_of('[');
        if(it != std::string::npos)
        {
            name.resize(it);
        }

        result.uniformLocations[name] = glGetUniformLocation(prog, name.c_str());
    }


    result.program = std::move(program);
    return result;
}

GLuint ShaderProgram::GetUniformLocation(const std::string& name) const
{
    auto it = uniformLocations.find(name);

    if(it != uniformLocations.end())
    {
        return it->second;
    }

    assert(it != uniformLocations.end());

    return ~0;
}