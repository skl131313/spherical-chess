
#include "opengl.hpp"

#include "../util.hpp"

#include "../json.hpp"
#include "../lodepng.h"

namespace
{
    enum PixelType
    {
        None,
        RGB8,
        RGBA8,
        RG16,
        Png,
    };

    template<typename T, int N>
    T ParseStringValue(const std::string& value, const T& defaultValue, const std::pair<const char*, T> (&values)[N])
    {
        for(auto& pair : values)
        {
            if(pair.first == value)
            {
                return pair.second;
            }
        }

        return defaultValue;
    }

    void LoadPixels(const std::string& filename, GLenum target, int level, PixelType pixelType, int width, int height)
    {
        switch(pixelType)
        {
        case PixelType::RGB8:
        {
            glTexImage2D(target, level, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Util::ReadBinaryFile(filename.c_str()).data());
            break;
        }
        case PixelType::RGBA8:
        {
            glTexImage2D(target, level, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Util::ReadBinaryFile(filename.c_str()).data());
            break;
        }
        case PixelType::RG16:
        {
            glTexImage2D(target, level, GL_RG16, width, height, 0, GL_RG, GL_UNSIGNED_SHORT, Util::ReadBinaryFile(filename.c_str()).data());
            break;
        }
        case PixelType::Png:
        {
            std::vector<unsigned char> pixelData;
            unsigned int w, h;

            lodepng::decode(pixelData, w, h, filename, LCT_RGB, 8u);

            glTexImage2D(target, level, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelData.data());

            break;
        }
        }
    }
}

OpenGLTexture OpenGLTexture::Make(GLenum target)
{
    OpenGLTexture result;

    result.textureTarget = target;
    glGenTextures(1, &result.texture);

    return result;
}

OpenGLTexture OpenGLTexture::MakeFromFile(const char* filename)
{
    OpenGLTexture result;

    glGenTextures(1, &result.texture);

    using Json = nlohmann::json;
    Json json = Json::parse(std::ifstream(filename));

    std::string type = json["type"];

    PixelType pixelType = ParseStringValue<PixelType>(json["format"].get<std::string>(), PixelType::None,
    {
        { "rgba8", PixelType::RGBA8 },
        { "rgb8",  PixelType::RGB8  },
        { "rg16",  PixelType::RG16  },
        { "png",   PixelType::Png   },
    });

    int width  = json["width"];
    int height = json["height"];
    int mipmapsCount = json["mipmapsCount"];

    // todo make these parameters part of the json input

        

    if(type == "cubemap")
    {
        result.textureTarget = GL_TEXTURE_CUBE_MAP;

        result.Bind();
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        if(mipmapsCount == 0)
        {
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, mipmapsCount);

        }

        for(auto& texture : json["textures"])
        {
            std::string side = texture["side"];

            GLenum t;

                 if(side == "+x") t = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
            else if(side == "+y") t = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
            else if(side == "+z") t = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
            else if(side == "-x") t = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
            else if(side == "-y") t = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
            else if(side == "-z") t = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

            int level = 0;
            for(std::string mipmap : texture["mipmaps"])
            {
                LoadPixels(mipmap, t, level, pixelType, width >> level, height >> level);
                level += 1;
            }

        }


    }
    else if(type == "2d")
    {
        result.textureTarget = GL_TEXTURE_2D;

        result.Bind();
            
        if(mipmapsCount == 0)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmapsCount);

        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        int level = 0;
        for(std::string mipmap : json["mipmaps"])
        {
            LoadPixels(mipmap, GL_TEXTURE_2D, level, pixelType, width >> level, height >> level);
            level += 1;
        }
    }

    glBindTexture(result.textureTarget, 0);

    return result;
}