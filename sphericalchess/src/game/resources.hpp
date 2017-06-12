
#pragma once

#include "../modeldata.hpp"
#include "../collision.hpp"

#include "board.hpp"
#include "font.hpp"
#include "shaders.hpp"

#include "../opengl/opengl.hpp"
#include "../core.hpp"

#include <SDL.h>

//! @todo move somewhere better..
struct Resources
{

    using Vertex = ModelData::Geometry::Vertex;


    struct Location
    {
        Mat4 transform;
        Mat4 inverse;
    };

    struct VertexDesc
    {
        GLint   first;
        GLsizei count;
    };

    struct Piece
    {
        VertexDesc vertexDesc;
        Collision collision;

        void Load(const char* filename, std::vector<Vertex>& vertices);
    };

    SDL_Window*   window  = nullptr;
    SDL_GLContext context = nullptr;

    ShaderProgram basicShader;
    ShaderProgram texShader;
    ShaderProgram fontShader;
    ShaderProgram pbrShader;

    Font font = Font(fontShader);

    OpenGLTexture envmap;
    OpenGLTexture irradiance;
    OpenGLTexture splitsum;

    OpenGLVertexArray vao;
    OpenGLBuffer vbo;

    VertexDesc boardTop;
    VertexDesc boardBot;
    VertexDesc board[Board::kDimension];
    Location locations[Board::kDimension][Board::kDimension];

    Piece pawn;
    Piece rook;
    Piece knight;
    Piece bishop;
    Piece queen;
    Piece king;


    void Load();

};