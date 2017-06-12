
#pragma once

namespace Math
{
    
template<typename Scalar, typename C, int Size>
auto operator * (Scalar s, const Vector<C, Size>& v)
{
    return v * s;
}

template<typename Scalar, typename C>
auto operator / (Scalar s, const Vector2<C>& v) -> Vector2<decltype(s / v.x)>
{
    return { s / v.x, s / v.y };
}

template<typename Scalar, typename C>
auto operator / (Scalar s, const Vector3<C>& v) -> Vector3<decltype(s / v.x)>
{
    return { s / v.x, s / v.y, s / v.z };
}

template<typename Scalar, typename C>
auto operator / (Scalar s, const Vector4<C>& v) -> Vector4<decltype(s / v.x)>
{
    return { s / v.x, s / v.y, s / v.z, s / v.w };
}

// todo try and isolate where windows.h is being included instead...
#undef min
#undef max

template<typename T, int Size>
Vector<T, Size> Min(const Vector<T, Size>& a, const Vector<T, Size>& b)
{
    Vector<T, Size> result;

    result[0] = std::min(a[0], b[0]);
    result[1] = std::min(a[1], b[1]);

    if(Size >= 3) result[2] = std::min(a[2], b[2]);
    if(Size >= 4) result[3] = std::min(a[3], b[3]);

    return result;
}

template<typename T, int Size>
Vector<T, Size> Max(const Vector<T, Size>& a, const Vector<T, Size>& b)
{
    Vector<T, Size> result;

    result[0] = std::max(a[0], b[0]);
    result[1] = std::max(a[1], b[1]);

    if(Size >= 3) result[2] = std::max(a[2], b[2]);
    if(Size >= 4) result[3] = std::max(a[3], b[3]);

    return result;
}

template<typename T, int Size>
std::pair<Vector<T, Size>, Vector<T, Size>> MinMax(const Vector<T, Size>& a, const Vector<T, Size>& b)
{
    return { Min(a, b), Max(a, b) };
}

//! @brief Perpendicular dot product.
//!
//! Follows the right hand rule similarly to @ref Cross() , as this is essentially the product of
//! the Z component from the cross product. 
template<typename T>
auto PerpDot(const Vector2<T>& a, const Vector2<T>& b)
{
    return a.x * b.y - a.y * b.x;
}

//! @brief Vector dot product.
template<typename T>
auto Dot(const Vector2<T>& a, const Vector2<T>& b)
{
    return a.x * b.x + a.y * b.y;
}

//! @brief Vector dot product.
template<typename T>
auto Dot(const Vector3<T>& a, const Vector3<T>& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

//! @brief Vector dot product.
template<typename T>
auto Dot(const Vector4<T>& a, const Vector4<T>& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

//! @brief Vector cross product, follows right hand rule.
template<typename T>
auto Cross(const Vector3<T>& a, const Vector3<T>& b)
{
    return Vector3<T>{ a.y * b.z - a.z * b.y,
                       a.z * b.x - a.x * b.z,
                       a.x * b.y - a.y * b.x  };
}

//! @brief Calculates the length of a vector squared.
template<typename T, int S>
auto LengthSqr(const Vector<T, S>& v)
{
    return Dot(v, v);
}

//! @brief Calculates the length of a vector.
template<typename T, int S, typename = std::enable_if<std::is_floating_point<T>::value>::type>
auto Length(const Vector<T, S>& v)
{
    return std::sqrt(Dot(v, v));
}

//! @brief Normalizes a vector to a unit vector.
//! @param [in,out] v The vector that will be normalized, not modified if length of vector nears zero.
//! @returns The length of the vector @p v, but if vector length nears zero (within 0.0001f) then returns zero.
template<typename T, int S, typename = std::enable_if<std::is_floating_point<T>::value>::type>
auto Normalize(Vector<T, S>& v)
{
    T length = Length(v);

    if(length < T(0.0001))
    {
        return T(0);
    }

    v /= length;

    return length;

}
}
