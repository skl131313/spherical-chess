
#include "font.hpp"

#include "shaders.hpp"

#include "impl/packagebinarytree.hpp"

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>

#include <functional>
#include <vector>
#include <numeric>
#include <iostream> // todo remove

namespace
{
    struct RunAtScopeEnd
    {


        RunAtScopeEnd(std::function<void()>&& function) : function(function)
        {
        }

        RunAtScopeEnd(const RunAtScopeEnd&) = delete;
        RunAtScopeEnd(RunAtScopeEnd&&)      = delete;

        RunAtScopeEnd& operator = (RunAtScopeEnd) = delete;

        ~RunAtScopeEnd()
        {
            function();
        }

    private:

        std::function<void()> function;
    };
}


void Font::Load(const char* trueTypeFilename, int size)
{
    FT_Library library;
    FT_Face    face;

    if(FT_Error error = FT_Init_FreeType(&library))
    {
        throw std::runtime_error("Failed to initialize freetype library.");
    }

    RunAtScopeEnd freeLibrary = [&library] { FT_Done_FreeType(library); };

   
    if(FT_Error error = FT_New_Face(library, trueTypeFilename, 0, &face))
    {
        throw std::runtime_error("Failed to create new freetype face.");
    }

    RunAtScopeEnd freeFace = [&face] { FT_Done_Face(face); };


    FT_Set_Char_Size(face, size * 64, size * 64, 96, 96);


    verticalSpacing = face->size->metrics.height / 64;
    verticalAscender = face->size->metrics.ascender / 64;
    verticalDescender = face->size->metrics.descender / 64;

    std::vector<u8> glyphPixels[NumGlyphs];
    int glyphSortedIndices[NumGlyphs];
    
    std::iota(glyphSortedIndices, glyphSortedIndices + NumGlyphs, 0);

    for(unsigned i = 0; i < NumGlyphs; ++i)
    {
        FT_Glyph glyph;
        Glyph&   glyphData = glyphs[i];
         
        if(FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT))
        {
            continue;
        }

        if(FT_Get_Glyph(face->glyph, &glyph))
        {
            continue;
        }

        RunAtScopeEnd freeGlyph = [&glyph] { FT_Done_Glyph(glyph); };

        if(FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true))
        {
            continue;
        }

        FT_BitmapGlyph bitmapGlyph = FT_BitmapGlyph(glyph);
        FT_Bitmap&     bitmap      = bitmapGlyph->bitmap;

        
        glyphData.size    = { int(bitmap.width), int(bitmap.rows) };
        glyphData.bearing = { bitmapGlyph->left, -bitmapGlyph->top };
        glyphData.advance = face->glyph->metrics.horiAdvance / 64; // todo use float and round?


        std::vector<u8>& pixels = glyphPixels[i];

        pixels.resize(glyphData.size.x * glyphData.size.y);

        std::copy(bitmap.buffer, bitmap.buffer + pixels.size(), pixels.begin());

    }


    std::sort(glyphSortedIndices, glyphSortedIndices + std::size(glyphSortedIndices), [this](int i, int j)
    {
        Glyph& a = glyphs[i];
        Glyph& b = glyphs[j];

        return a.size.x * a.size.y > b.size.x * b.size.y; // sort by area in *descending* order
    });


    Impl::PackageBinaryTree tree({ 4096, 4096 });

    for(int i : glyphSortedIndices)
    {
        Glyph& glyph = glyphs[i];

        if(glyph.size.x * glyph.size.y == 0)
        {
            continue;
        }

        int nodeIndex = tree.Insert(glyph.size, i);

        if(nodeIndex == Impl::PackageBinaryTree::None)
        {
            throw std::runtime_error("Unable to insert character code '" + std::to_string(i) + "' into font bitmap.");
        }
    }

    textureSize = tree.GetMinSize();

    std::vector<u8> pixels(textureSize.x * textureSize.y);

    for(auto& node : tree.GetNodes())
    {
        if(node.id == Impl::PackageBinaryTree::None)
        {
            continue;
        }

        Glyph& glyph = glyphs[node.id];
        auto&  nodePixels = glyphPixels[node.id];

        glyph.coord = node.rect.Position();

        for(int y = 0; y < glyph.size.y; ++y)
        {
            auto begin = nodePixels.begin() + (y * glyph.size.x);
            auto dest  = pixels.begin() + (node.rect.x + (y + node.rect.y) * textureSize.x);

            std::copy(begin, begin + glyph.size.x, dest);
            
        }
    }

    texture = OpenGLTexture::Make(GL_TEXTURE_2D);
    texture.Bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, textureSize.x, textureSize.y, false, GL_RED, GL_UNSIGNED_BYTE, pixels.data());
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glBindTexture(GL_TEXTURE_2D, 0);



    vao = OpenGLVertexArray::Make();
    vbo = OpenGLBuffer::Make();

    vao.Bind();
    vbo.Bind(GL_ARRAY_BUFFER);

    glEnableVertexAttribArray(ShaderAttribPosition);
    glEnableVertexAttribArray(ShaderAttribCoord);

    glVertexAttribPointer(ShaderAttribPosition, 2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
    glVertexAttribPointer(ShaderAttribCoord,    2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, coord)));

    glBindVertexArray(0);

}

void Font::Draw(Rect2i window, Vec2i position, Align anchor, const std::string& text)
{
    using namespace std::string_literals;

    std::vector<Vertex> vertices;
    vertices.reserve(text.size() * 6);


    Mat4 mat = Math::Ortho(window.Left(), window.Right(), window.Bot(), window.Top()); // todo Math::Ortho(Rect)?


    Vec2i vertexOffset{ 0, verticalAscender };
    
    Vec2 upper{ 0, 0 };

    for(char c : text)
    {
        if(c < 0)
        {
            continue;
        }

        switch(c)
        {
        case '\n':
        {
            vertexOffset.x = 0;
            vertexOffset.y += verticalSpacing; // todo implement spacing variable?
            continue;
        }
        }

        Glyph& glyph = glyphs[c];


        if(glyph.size.x * glyph.size.y > 0)
        {
            const Rect2 rect = Rect2(Vec2(vertexOffset + glyph.bearing), Vec2(glyph.size));
            const Rect2 uv   = Rect2(Vec2(glyph.coord) / textureSize, Vec2(glyph.size) / textureSize);

            vertices.push_back({ rect.LeftTop(),  uv.LeftTop() });
            vertices.push_back({ rect.LeftBot(),  uv.LeftBot() });
            vertices.push_back({ rect.RightBot(), uv.RightBot() });

            vertices.push_back({ rect.LeftTop(),  uv.LeftTop()  });
            vertices.push_back({ rect.RightBot(), uv.RightBot() });
            vertices.push_back({ rect.RightTop(), uv.RightTop() });

            upper.x = std::max(upper.x, rect.Right());
        }


        vertexOffset.x += glyph.advance;
    }

    upper.y = vertexOffset.y - verticalDescender;

    Rect2 textRect = Rect2::FromBounds(Vec2(0), upper);

    Vec2 offset = Vec2(position);

    switch(anchor)
    {
    case Align::LeftTop:  offset += window.LeftTop()    - textRect.LeftTop();    break;
    case Align::Left:     offset += window.LeftCenter() - textRect.LeftCenter(); break;
    case Align::LeftBot:  offset += window.LeftBot()    - textRect.LeftBot();    break;

    case Align::Top:      offset += window.CenterTop() - textRect.CenterTop(); break;
    case Align::Center:   offset += window.Center()    - textRect.Center();    break;
    case Align::Bot:      offset += window.CenterBot() - textRect.CenterBot(); break;

    case Align::RightTop: offset += window.RightTop()    - textRect.RightTop();    break;
    case Align::Right:    offset += window.RightCenter() - textRect.RightCenter(); break;
    case Align::RightBot: offset += window.RightBot()    - textRect.RightBot();    break;
    }


    vao.Bind();
    vbo.Bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();


    shader.UseProgram();
    shader.SetUniform("diffuseTexture"s, 0);
    shader.SetUniform("viewproj"s, mat);
    shader.SetUniform("offset"s, offset);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(0);

}

