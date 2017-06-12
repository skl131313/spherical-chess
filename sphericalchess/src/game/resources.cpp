
#include "resources.hpp"

void Resources::Piece::Load(const char* filename, std::vector<Vertex>& vertices)
{
    ModelData model;

    model.Load(filename);

    vertexDesc.first = GLint(vertices.size());
    vertexDesc.count = 0;

    if(auto geometry = model.FindGeometry("piece"))
    {
        vertexDesc.count = GLsizei(geometry->vertices.size());
        vertices.insert(vertices.end(), geometry->vertices.begin(), geometry->vertices.end());
    }

    collision.Load(model.GetCollision());
}

void Resources::Load()
{
    // todo delete other value incase Load() is called twice, to reload maybe

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
    int width = 1280;
    int height = 720;

    window  = SDL_CreateWindow("Spherical Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);

    if(!gladLoadGL())
    {
        throw "glad fail"; // todo change...
    }

    vbo = OpenGLBuffer::Make();
    vao = OpenGLVertexArray::Make();

    basicShader = ShaderProgram::MakeFromFiles("data/shaders/basic.vert", "data/shaders/basic.frag");
    texShader   = ShaderProgram::MakeFromFiles("data/shaders/tex.vert",   "data/shaders/tex.frag");
    fontShader  = ShaderProgram::MakeFromFiles("data/shaders/font.vert",  "data/shaders/font.frag");
    pbrShader   = ShaderProgram::MakeFromFiles("data/shaders/pbr.vert",   "data/shaders/pbr.frag");

    font.Load("data/fonts/orbitron.ttf", 32); // todo Font::Make instead

    envmap     = OpenGLTexture::MakeFromFile("data/textures/envmap.json");
    irradiance = OpenGLTexture::MakeFromFile("data/textures/irradiance.json");
    splitsum   = OpenGLTexture::MakeFromFile("data/textures/splitsum.json");


    std::vector<Vertex> vertices;

    ModelData model;
        
    model.Load("data/models/model.scmodel");

    if(auto geometry = model.FindGeometry("boardtop"))
    {
        boardTop.first = GLint(vertices.size());
        boardTop.count = GLsizei(geometry->vertices.size());

        vertices.insert(vertices.end(), geometry->vertices.begin(), geometry->vertices.end());
    }

    if(auto geometry = model.FindGeometry("boardbot"))
    {
        boardBot.first = GLint(vertices.size());
        boardBot.count = GLsizei(geometry->vertices.size());

        vertices.insert(vertices.end(), geometry->vertices.begin(), geometry->vertices.end());
    }

    for(int j = 0; j < Board::kDimension; ++j)
    {
        if(auto marker = model.FindMarker("location" + std::to_string(j)))
        {
            for(int i = 0; i < Board::kDimension; ++i)
            {
                Vec3 side = Math::Cross(marker->up, marker->forward);

                Math::Normalize(side);

                Mat3 rotation = Mat3(marker->forward, side, marker->up);

#if 1
                // todo, refactor this, Math::Inverse(Mat4x3) maybe ? or a "Transform" class with a Mat3 and Vec3 for position ?

                Mat3 delta = Math::EulerAngleZ(-45.0_deg * i);
                Mat3 deltaInverse = Math::Transpose(delta);

                Mat4 mat = Mat4(rotation);
                mat[3] = Vec4(marker->position, 1.0f);
                locations[i][j].transform = Mat4(delta) * mat;
                    

                mat = Mat4(Math::Transpose(rotation));
                mat[3] = Vec4(Mat3(mat) * -marker->position, 1.0f);
#endif

                locations[i][j].inverse = mat * Mat4(deltaInverse);
            }
        }

        if(auto geometry = model.FindGeometry("board" + std::to_string(j)))
        {
            board[j].first = GLint(vertices.size());
            board[j].count = GLsizei(geometry->vertices.size());

            vertices.insert(vertices.end(), geometry->vertices.begin(), geometry->vertices.end());
        }
    }

    pawn  .Load("data/models/piece.scmodel",  vertices); // todo Piece::Make() instead, for consistency
    rook  .Load("data/models/rook.scmodel",   vertices);
    knight.Load("data/models/knight.scmodel", vertices);
    bishop.Load("data/models/bishop.scmodel", vertices);
    queen .Load("data/models/queen.scmodel",  vertices);
    king  .Load("data/models/king.scmodel",   vertices);

    vao.Bind();
    vbo.Bind(GL_ARRAY_BUFFER);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(ShaderAttribPosition);
    glEnableVertexAttribArray(ShaderAttribNormal);
    glEnableVertexAttribArray(ShaderAttribCoord);

    glVertexAttribPointer(ShaderAttribPosition, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
    glVertexAttribPointer(ShaderAttribNormal,   3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glVertexAttribPointer(ShaderAttribCoord,    2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glBindVertexArray(0);

}