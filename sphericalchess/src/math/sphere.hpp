#pragma once

#include "vector.hpp"

namespace Math
{

//! @brief Simple representation of a sphere.
struct Sphere
{
    Vec3 center;
    float radius;
};


//! @brief Calculates the closest intersection to the @p start of a line with a Sphere.
//! @param [in]  sphere        The sphere to test intersection against.
//! @param [in]  start         The beginning point of the line.
//! @param [in]  end           The end point of the line.
//! @param [out] outputPercent The percentage between start and end for which the intersection occurs,
//!                            if an intersection does not exist this value is not modified.
//! @returns True if an intersection exists, otherwise false.
inline bool CalculateIntersection(const Sphere& sphere, const Vec3& start, const Vec3& end, float& outputPercent)
{

    Vec3 direction = end - start;
    float directionLength = Normalize(direction);

    Vec3 centerToStart = sphere.center - start;

    float v = Dot(centerToStart, direction);
    float d = sphere.radius * sphere.radius - (LengthSqr(centerToStart) - v*v);

    if(d < 0.0f) return false;

    float p = v - std::sqrt(d);

    if(p > directionLength) return false;

    outputPercent = p / directionLength;

    return true;
}

}