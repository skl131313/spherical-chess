
#pragma once

#include "opengl.hpp"

#include "../core.hpp"

#include <unordered_map>
#include <string>

#if 0

    void SetArray(int n, const int*   v) { glUniform1iv(location, n, v); }
    void SetArray(int n, const float* v) { glUniform1fv(location, n, v); }
    void SetArray(int n, const Vec2* v)  { glUniform2fv(location, n, reinterpret_cast<const GLfloat*>(v)); }
    void SetArray(int n, const Vec3* v)  { glUniform3fv(location, n, reinterpret_cast<const GLfloat*>(v)); }
    void SetArray(int n, const Vec4* v)  { glUniform4fv(location, n, reinterpret_cast<const GLfloat*>(v)); }
    void SetArray(int n, const Mat3* v)  { glUniformMatrix3fv(location, n, false, reinterpret_cast<const GLfloat*>(v)); }
    void SetArray(int n, const Mat4* v)  { glUniformMatrix4fv(location, n, false, reinterpret_cast<const GLfloat*>(v)); }

#endif

class ShaderProgram
{
public:

    static ShaderProgram MakeFromFiles(const char* vertexFilename, const char* fragmentFilename);

    ShaderProgram() = default;
    ShaderProgram(ShaderProgram&& other)
    {
        program = std::move(other.program);
        uniformLocations = std::move(other.uniformLocations);
    }

    ShaderProgram& operator = (ShaderProgram other)
    {
        program = std::move(other.program);
        uniformLocations = std::move(other.uniformLocations);
        return *this;
    }

    void UseProgram()
    {
        glUseProgram(program.Value());
    }

    void SetUniform(const std::string& name, const int&   v) { glUniform1iv(GetUniformLocation(name), 1, &v); }
    void SetUniform(const std::string& name, const float& v) { glUniform1fv(GetUniformLocation(name), 1, &v); }
    void SetUniform(const std::string& name, const Vec2& v)  { glUniform2fv(GetUniformLocation(name), 1, &v.x); }
    void SetUniform(const std::string& name, const Vec3& v)  { glUniform3fv(GetUniformLocation(name), 1, &v.x); }
    void SetUniform(const std::string& name, const Vec4& v)  { glUniform4fv(GetUniformLocation(name), 1, &v.x); }
    void SetUniform(const std::string& name, const Mat3& v)  { glUniformMatrix3fv(GetUniformLocation(name), 1, false, &v[0][0]); }
    void SetUniform(const std::string& name, const Mat4& v)  { glUniformMatrix4fv(GetUniformLocation(name), 1, false, &v[0][0]); }

private:

    struct Program
    {
        static Program Make()
        {
            return Program(glCreateProgram());
        }

        ~Program()
        {
            if(program) glDeleteProgram(program);
        }

        Program()               = default;
        Program(const Program&) = delete;

        Program(Program&& other)
        {
            std::swap(program, other.program);
        }

        Program& operator = (Program other)
        {
            std::swap(program, other.program);
            return *this;
        }

        GLuint Value() { return program; }

    private:

        explicit Program(GLuint program) : program(program)
        {
        }

        GLuint program = 0;
    };


    Program program;
    std::unordered_map<std::string, GLuint> uniformLocations;

    GLuint GetUniformLocation(const std::string& name) const;

};


