
#include "modeldata.hpp"

#include "util.hpp"

// todo this is horribly implemented
struct BinaryFile
{
    BinaryFile() = default;

    void Open(const char* filename)
    {
        file.open(filename, std::ios::binary);
    }

    template<typename T> T Read()
    {
        // static_assert(std::is_integral<T>::value, "Not integral.. todo");

        T value = 0;

        file.read(reinterpret_cast<char*>(&value), sizeof(T));

        return value;
    }

    template<>
    std::string Read<std::string>()
    {
        auto size = Read<u8>();

        std::string str;

        str.reserve(size);

        for(int i = 0; i < size; ++i)
        {
            str.push_back(file.get());
        }

        return str;
    }

    template<>
    Vec2 Read<Vec2>()
    {
        Vec2 result;

        result.x = Read<float>();
        result.y = Read<float>();

        return result;
    }

    template<>
    Vec3 Read<Vec3>()
    {
        Vec3 result;

        result.x = Read<float>();
        result.y = Read<float>();
        result.z = Read<float>();

        return result;
    }

    template<>
    Vec4 Read<Vec4>()
    {
        Vec4 result;

        result.x = Read<float>();
        result.y = Read<float>();
        result.z = Read<float>();
        result.w = Read<float>();

        return result;
    }

private:

    std::ifstream file;
};

void ModelData::Load(const char* filename)
{
    markers.clear();
    geometries.clear();
    collision.triangles.clear();

    BinaryFile file;

    file.Open(filename);

    u32 markersCount = file.Read<u32>();

    for(u32 i = 0; i < markersCount; ++i)
    {
        Marker marker;

        std::string name = file.Read<std::string>();

        marker.position = file.Read<Vec3>();
        marker.forward  = file.Read<Vec3>();
        marker.up       = file.Read<Vec3>();

        markers.insert(std::make_pair(std::move(name), marker));
    }

    u32 geometriesCount = file.Read<u32>();

    for(u32 i = 0; i < geometriesCount; ++i)
    {
        std::string name = file.Read<std::string>();

        Geometry& geometry = geometries[name];

        geometry.vertices.clear(); // todo handle multiple same name geometries better ?

        u32 vertexCount = file.Read<u32>();

        geometry.vertices.reserve(vertexCount);

        for(u32 j = 0; j < vertexCount; ++j)
        {
            Geometry::Vertex vertex;

            vertex.position = file.Read<Vec3>();
            vertex.normal   = file.Read<Vec3>();
            vertex.uv       = file.Read<Vec2>();

            geometry.vertices.push_back(vertex);
        }
    }

    u32 collisionVertices = file.Read<u32>();

    collision.triangles.reserve(collisionVertices);

    for(u32 i = 0; i < collisionVertices; ++i)
    {
        Collision::Triangle triangle;

        for(int j = 0; j < 3; ++j)
        {
            auto& p = triangle.points[j];

            p.x = file.Read<f32>();
            p.y = file.Read<f32>();
            p.z = file.Read<f32>();
        }

        collision.triangles.push_back(triangle);
    }
}

auto ModelData::FindMarker(const std::string& name) const -> const Marker*  
{
    auto it = markers.find(name);

    if(it != markers.end())
    {
        return &it->second;
    }

    return nullptr;
}

auto ModelData::FindGeometry(const std::string& name) const -> const Geometry*
{
    auto it = geometries.find(name);

    if(it != geometries.end())
    {
        return &it->second;
    }

    return nullptr;
}