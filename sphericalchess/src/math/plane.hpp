#pragma once

#include "vector.hpp"

namespace Math
{

//! @brief An infinite plane representation.
struct Plane3
{
    Vec3 normal; //!< The unit vector normal of the plane.
    float d;     //!< The D of the plane, where 'normal * d' is a point on the plane. 
};

//! @brief Creates a Plane3 from 3 points, using CCW rotation of points for the front face of the plane.
inline Plane3 ConstructPlaneFromPoints(const Vec3& a, const Vec3& b, const Vec3& c)
{
    Plane3 plane;

    Vec3 ab = b - a;
    Vec3 ac = c - a;

    plane.normal = Cross(ab, ac); // CCW order of points for front face

    Normalize(plane.normal);

    plane.d = Dot(plane.normal, a);

    return plane;
}

//! @brief Calculates the distance between a Plane3 and a point.
inline float Distance(const Plane3& plane, const Vec3& point)
{
    return Dot(plane.normal, point) - plane.d;
}

}