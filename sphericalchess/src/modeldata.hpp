
#pragma once

#include "core.hpp"

#include <string>
#include <istream>
#include <unordered_map>
#include <vector>

class ModelData
{
public:
    
    struct Marker
    {
        Vec3 position;
        Vec3 forward;
        Vec3 up;
    };

    struct Geometry
    {
        struct Vertex
        {
            Vec3 position;
            Vec3 normal;
            Vec2 uv;
        };

        std::vector<Vertex> vertices;
    };

    struct Collision
    {
        struct Triangle
        {
            Vec3 points[3];
        };

        std::vector<Triangle> triangles;

    };

    ModelData() = default;

    void Load(const char* filename);

    const Marker*   FindMarker(const std::string& name) const;
    const Geometry* FindGeometry(const std::string& name) const;

    const Collision& GetCollision() const { return collision; }

private:

    std::unordered_map<std::string, Marker> markers;
    std::unordered_map<std::string, Geometry> geometries;
    Collision collision;
};
