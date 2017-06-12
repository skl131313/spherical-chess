
#pragma once

#include <cmath>

#include "constants.hpp"

//! Mathematical library with various functions and classes.
namespace Math
{

//! User defined literial operators for mathematical values.
namespace LiteralOperators
{
    //! @brief User literial to convert degrees into radians, only supports 'float' output.
    constexpr float operator"" _deg(long double degrees)
    {
        return float(degrees) * Pi<float>() / 180.0f;
    }
}

//! @brief Used to square a value.
//! @param [in] value  The value to square.
//! @returns Essentially (value * value).
template<typename T>
T Sqr(const T& value)
{
    return value * value;
}

//! @brief Used to clamp values within bounds of min and max of generic type.
//! @param [in] value The value to check is within bounds.
//! @param [in] min   The minimum bounds to test against value, must be less than max.
//! @param [in] max   The maximum bounds to test against value, must be more than min.
//! @returns The clamped value in bounds [min, max].
template<typename T>
T Clamp(const T& value, const T& min, const T& max)
{
    if(value < min) return min;
    if(value > max) return max;

    return value;
}

}


#include "vector.hpp"
#include "vectorfunctions.hpp"

#include "plane.hpp"
#include "rect.hpp"

#include "matrix.hpp"
#include "matrixfunctions.hpp"

#include "sphere.hpp"
