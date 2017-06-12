
#include "collision.hpp"



namespace
{

    Collision::Axis CalculateProjectionAxis(const Vec3& normal)
    {
        using Axis = Collision::Axis;

        float x = std::abs(normal.x);
        float y = std::abs(normal.y);
        float z = std::abs(normal.z);

        if(x >= y && x >= z)
        {
            if(normal.x < 0) return Axis::NegX;
            else             return Axis::PosX;
        }
        else if(y >= z)
        {
            if(normal.y < 0) return Axis::NegY;
            else             return Axis::PosY;
        }
        else
        {
            if(normal.z < 0) return Axis::NegZ;
            else             return Axis::PosZ;        
        }

    }

    Vec2 Project(Collision::Axis axis, const Vec3& point)
    {
        using Axis = Collision::Axis;

        switch(axis)
        {
        case Axis::PosX: return Vec2(point.y, point.z);
        case Axis::NegX: return Vec2(point.z, point.y);
        case Axis::PosY: return Vec2(point.z, point.x);
        case Axis::NegY: return Vec2(point.x, point.z);
        case Axis::PosZ: return Vec2(point.x, point.y);
        case Axis::NegZ: return Vec2(point.y, point.x);
        }

        return { 0.0f, 0.0f };
    }

    bool CheckInsidePoints(const Vec2& point, const Vec2 points[3])
    {
        for(int i = 0; i < 3; ++i)
        {
            Vec2 a = points[i] - point;
            Vec2 b = points[(i + 1) % 3] - point;

            if(Math::PerpDot(a, b) < 0)
            {
                return false;
            }
        }

        return true;
    }
}

void Collision::Load(const ModelData::Collision& collision)
{
    triangles.clear();
    triangles.reserve(collision.triangles.size());
        
    // todo possible to use polygons here instead
    // eg: a flat circle would be one polygon instead of multiple triangles

    for(auto& t : collision.triangles)
    {
        Triangle triangle;

        triangle.plane = Math::ConstructPlaneFromPoints(t.points[0], t.points[1], t.points[2]);
        triangle.axis = CalculateProjectionAxis(triangle.plane.normal);

        for(int i = 0; i < 3; ++i)
        {
            triangle.points[i] = Project(triangle.axis, t.points[i]);
        }

        triangles.push_back(triangle);
    }

}

bool Collision::Collide(const Vec3& start, const Vec3& end, float percentStart, float& output) const
{
    float percent = percentStart;
    bool hit = false;

    Vec3 segment = end - start;

    for(int i = 0; i < triangles.size(); ++i)
    {
        auto& triangle = triangles[i];

        float s = Math::Dot(triangle.plane.normal, segment);

        if(s >= 0)
        {
            continue;
        }

        float p = Math::Distance(triangle.plane, start) / -s;
            
        if(p > percent)
        {
            continue;
        }

        Vec3 point = start + segment * p;

        if(CheckInsidePoints(Project(triangle.axis, point), triangle.points))
        {
            hit = true;
            percent = p;
        }

    }

    if(hit)
    {
        output = percent;
    }

    return hit;

}