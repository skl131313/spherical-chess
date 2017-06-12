
#pragma once


#include "modeldata.hpp"
#include "math/math.hpp"

class Collision
{
public:

    enum class Axis
    {
        PosX, NegX,
        PosY, NegY,
        PosZ, NegZ,
    };

    void Load(const ModelData::Collision& collision);
    bool Collide(const Vec3& start, const Vec3& end, float percentStart, float& output) const;

private:

    struct Triangle
    {
        Axis axis;
        Plane3 plane;

        Vec2 points[3];
    };

    std::vector<Triangle> triangles;
    
};
