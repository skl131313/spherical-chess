
#pragma once

#include "../opengl/opengl.hpp"

#include <string>


class Font
{
public:

    enum struct Align
    {
        LeftTop, Top,    RightTop,
        Left,    Center, Right,
        LeftBot, Bot,    RightBot
    };

    explicit Font(ShaderProgram& shader) : shader(shader)
    {
    }

    void Load(const char* trueFontFilename, int size);
    void Draw(Rect2i window, Vec2i position, Align anchor, const std::string& text);

private:

    struct Glyph
    {
        Vec2i size     = { 0, 0 };
        Vec2i coord    = { 0, 0 };
        Vec2i bearing  = { 0, 0 };

        int advance    = 0;
    };

    struct Vertex
    {
        Vec2 position;
        Vec2 coord;
    };

    static const int NumGlyphs = 128;

    ShaderProgram& shader;

    Glyph glyphs[NumGlyphs];

    OpenGLVertexArray vao;
    OpenGLBuffer vbo;
    OpenGLTexture texture;

    Vec2i textureSize = { 0, 0 };

    int verticalSpacing   = 0;
    int verticalAscender  = 0;
    int verticalDescender = 0;

};