
#pragma once

#include <tuple>

namespace Math
{

//! A template column vector supporting sizes of 2, 3, or 4.
template<typename C, int Size> struct Vector;

template<typename T> using Vector2 = Vector<T, 2>;
template<typename T> using Vector3 = Vector<T, 3>;
template<typename T> using Vector4 = Vector<T, 4>;


template<typename C>
struct Vector<C, 2>
{
    static constexpr int NumComponents = 2;
    using Component = C;


    Component x;
    Component y;

    Vector() = default;

    explicit Vector(Component xy) : x(xy), y(xy)
    {
    }

    Vector(Component x, Component y) : x(x), y(y)
    {
    }

    template<typename T, int S, typename = std::enable_if<(S >= NumComponents)>::type>
    explicit Vector(const Vector<T, S>& a) : x(a.x), y(a.y)
    {
    }

    template<typename = std::enable_if<std::is_integral<Component>::value>::type>
    bool operator == (const Vector& a) const
    {
        return x == a.x && y == a.y;
    }

    template<typename = std::enable_if<std::is_integral<Component>::value>::type>
    bool operator != (const Vector& a) const
    {
        return x != a.x || y != a.y;
    }

    template<typename Scalar> auto operator * (Scalar s) const -> Vector2<decltype(x * s)> { return { x * s, y * s }; }
    template<typename Scalar> auto operator / (Scalar s) const -> Vector2<decltype(x / s)> { return { x / s, y / s }; }

    template<typename Scalar> Vector& operator *= (Scalar s) { x *= s; y *= s; return *this; }
    template<typename Scalar> Vector& operator /= (Scalar s) { x /= s; y /= s; return *this; }
    
    Vector operator - () const { return { -x, -y }; }

    template<typename T> auto operator + (const Vector2<T>& a) const -> Vector2<decltype(x + a.x)> { return { x + a.x, y + a.y }; }
    template<typename T> auto operator - (const Vector2<T>& a) const -> Vector2<decltype(x - a.x)> { return { x - a.x, y - a.y }; }
    template<typename T> auto operator * (const Vector2<T>& a) const -> Vector2<decltype(x * a.x)> { return { x * a.x, y * a.y }; }
    template<typename T> auto operator / (const Vector2<T>& a) const -> Vector2<decltype(x / a.x)> { return { x / a.x, y / a.y }; }

    template<typename T> Vector& operator += (const Vector2<T>& a) { x += a.x; y += a.y; return *this; }
    template<typename T> Vector& operator -= (const Vector2<T>& a) { x -= a.x; y -= a.y; return *this; }
    template<typename T> Vector& operator *= (const Vector2<T>& a) { x *= a.x; y *= a.y; return *this; }
    template<typename T> Vector& operator /= (const Vector2<T>& a) { x /= a.x; y /= a.y; return *this; }

    Component& operator [] (int i)
    {
        static_assert(sizeof(Vector) == sizeof(Component[NumComponents]), "Unexpected padding.");
        return (&x)[i];
    }

    const Component& operator [] (int i) const
    {
        static_assert(sizeof(Vector) == sizeof(Component[NumComponents]), "Unexpected padding.");
        return (&x)[i];
    }

};


template<typename C>
struct Vector<C, 3>
{
    static constexpr int NumComponents = 3;
    using Component = C;

    Component x;
    Component y;
    Component z;

    Vector() = default;
    
    explicit Vector(Component xyz) : x(xyz), y(xyz), z(xyz)
    {
    }

    Vector(Component x, Component y, Component z) : x(x), y(y), z(z)
    {
    }

    template<typename T>
    Vector(Vector2<T> a, Component z) : x(a.x), y(a.y), z(z)
    {
    }

    template<typename T, int S, typename = std::enable_if<(S >= NumComponents)>::type>
    explicit Vector(const Vector<T, S>& a) : x(a.x), y(a.y), z(a.z)
    {
    }

    
    template<typename = std::enable_if<std::is_integral<Component>::value>::type>
    bool operator == (const Vector& a) const
    {
        return x == a.x && y == a.y && z == a.z;
    }

    template<typename = std::enable_if<std::is_integral<Component>::value>::type>
    bool operator != (const Vector& a) const
    {
        return x != a.x || y != a.y || z != a.z;
    }

    template<typename Scalar> auto operator * (Scalar s) const -> Vector3<decltype(x * s)> { return { x * s, y * s, z * s }; }
    template<typename Scalar> auto operator / (Scalar s) const -> Vector3<decltype(x / s)> { return { x / s, y / s, z / s }; }

    template<typename Scalar> Vector& operator *= (Scalar s) { x *= s; y *= s; z *= s; return *this; }
    template<typename Scalar> Vector& operator /= (Scalar s) { x /= s; y /= s; z /= s; return *this; }
    
    Vector operator - () const { return { -x, -y, -z }; }

    template<typename T> auto operator + (const Vector3<T>& a) const -> Vector3<decltype(x + a.x)> { return { x + a.x, y + a.y, z + a.z }; }
    template<typename T> auto operator - (const Vector3<T>& a) const -> Vector3<decltype(x - a.x)> { return { x - a.x, y - a.y, z - a.z }; }
    template<typename T> auto operator * (const Vector3<T>& a) const -> Vector3<decltype(x * a.x)> { return { x * a.x, y * a.y, z * a.z }; }
    template<typename T> auto operator / (const Vector3<T>& a) const -> Vector3<decltype(x / a.x)> { return { x / a.x, y / a.y, z / a.z }; }

    template<typename T> Vector& operator += (const Vector3<T>& a) { x += a.x; y += a.y; z += a.z; return *this; }
    template<typename T> Vector& operator -= (const Vector3<T>& a) { x -= a.x; y -= a.y; z -= a.z; return *this; }
    template<typename T> Vector& operator *= (const Vector3<T>& a) { x *= a.x; y *= a.y; z *= a.z; return *this; }
    template<typename T> Vector& operator /= (const Vector3<T>& a) { x /= a.x; y /= a.y; z /= a.z; return *this; }

    Component& operator [] (int i)
    {
        static_assert(sizeof(Vector) == sizeof(Component[NumComponents]), "Unexpected padding.");
        return (&x)[i];
    }

    const Component& operator [] (int i) const
    {
        static_assert(sizeof(Vector) == sizeof(Component[NumComponents]), "Unexpected padding.");
        return (&x)[i];
    }

};

template<typename C>
struct Vector<C, 4>
{
    static constexpr int NumComponents = 4;
    using Component = C;

    Component x;
    Component y;
    Component z;
    Component w;

    Vector() = default;

    explicit Vector(Component xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw)
    {
    }

    Vector(Component x, Component y, Component z, Component w) : x(x), y(y), z(z), w(w)
    {
    }

    template<typename T>
    Vector(const Vector2<T>& a, Component z, Component w) : x(a.x), y(a.y), z(z), w(w)
    {
    }

    template<typename T>
    Vector(const Vector3<T>& a, Component w) : x(a.x), y(a.y), z(a.z), w(w)
    {
    }

    template<typename T>
    explicit Vector(const Vector4<T>& a) : x(a.x), y(a.y), z(a.z), w(a.w)
    {
    }

    template<typename = std::enable_if<std::is_integral<Component>::value>::type>
    bool operator == (const Vector& a) const
    {
        return x == a.x && y == a.y && z == a.z && w == a.w;
    }

    template<typename = std::enable_if<std::is_integral<Component>::value>::type>
    bool operator != (const Vector& a) const
    {
        return x != a.x || y != a.y || z != a.z || w != a.w;
    }

    template<typename Scalar> auto operator * (Scalar s) const -> Vector4<decltype(x * s)> { return { x * s, y * s, z * s, w * s }; }
    template<typename Scalar> auto operator / (Scalar s) const -> Vector4<decltype(x / s)> { return { x / s, y / s, z / s, w / s }; }

    template<typename Scalar> Vector& operator *= (Scalar s) { x *= s; y *= s; z *= s; w *= s; return *this; }
    template<typename Scalar> Vector& operator /= (Scalar s) { x /= s; y /= s; z /= s; w /= s; return *this; }
    
    Vector operator - () const { return { -x, -y, -z, -w }; }

    template<typename T> auto operator + (const Vector4<T>& a) const -> Vector4<decltype(x + a.x)> { return { x + a.x, y + a.y, z + a.z, w + a.w }; }
    template<typename T> auto operator - (const Vector4<T>& a) const -> Vector4<decltype(x - a.x)> { return { x - a.x, y - a.y, z - a.z, w - a.w }; }
    template<typename T> auto operator * (const Vector4<T>& a) const -> Vector4<decltype(x * a.x)> { return { x * a.x, y * a.y, z * a.z, w * a.w }; }
    template<typename T> auto operator / (const Vector4<T>& a) const -> Vector4<decltype(x / a.x)> { return { x / a.x, y / a.y, z / a.z, w / a.w }; }

    template<typename T> Vector& operator += (const Vector4<T>& a) { x += a.x; y += a.y; z += a.z; w += a.w; return *this; }
    template<typename T> Vector& operator -= (const Vector4<T>& a) { x -= a.x; y -= a.y; z -= a.z; w -= a.w; return *this; }
    template<typename T> Vector& operator *= (const Vector4<T>& a) { x *= a.x; y *= a.y; z *= a.z; w *= a.w; return *this; }
    template<typename T> Vector& operator /= (const Vector4<T>& a) { x /= a.x; y /= a.y; z /= a.z; w /= a.w; return *this; }

    Component& operator [] (int i)
    {
        static_assert(sizeof(Vector) == sizeof(Component[NumComponents]), "Unexpected padding.");
        return (&x)[i];
    }

    const Component& operator [] (int i) const
    {
        static_assert(sizeof(Vector) == sizeof(Component[NumComponents]), "Unexpected padding.");
        return (&x)[i];
    }
};

using Vec2i = Vector2<int>;

using Vec2 = Vector2<float>;
using Vec3 = Vector3<float>;
using Vec4 = Vector4<float>;


}