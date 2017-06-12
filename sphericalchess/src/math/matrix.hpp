
#pragma once

#include "vector.hpp"
#include <sstream>

namespace Math
{

class Mat4;

//! @brief A 3x3 matrix in column major order comprised of Vec3.
class Mat3
{
public:

    using ColumnType = Vec3;

    //! @brief Default constructed which is uninitialized, identity matrix can be constructed using [Mat3(1.0f)](@ref Mat3(float)) constructor.
    Mat3() = default;

    //! @brief Initializes diagonal values with @p d and the rest with zero.
    explicit Mat3(float d) : Mat3(d,0,0, 0,d,0, 0,0,d)
    {
    }

    //! @brief Initializes matrix with columns as input.
    Mat3(const ColumnType& c0, const ColumnType& c1, const ColumnType& c2) : m{c0, c1, c2}
    {
    }

    //! @brief Initializes matrix with each separate component.
    Mat3(float c00, float c01, float c02, float c10, float c11, float c12, float c20, float c21, float c22)
        : m{ ColumnType(c00, c01, c02), ColumnType(c10, c11, c12), ColumnType(c20, c21, c22) }
    {
    }

    //! @brief Extracts a Mat3 from a Mat4, extra components are ignored.
    explicit Mat3(const Mat4& m);

    ColumnType&       operator [] (int i)       { return m[i]; } //!< @brief Access column of matrix, no bounds checking.
    const ColumnType& operator [] (int i) const { return m[i]; } //!< @brief Access column of matrix, no bounds checking.

    // Mat3 operator * (float v) const { return Mat3(m[0] * v, m[1] * v, m[2] * v); } //!< @brief Scalar multiplication with matrix.
    // Mat3 operator / (float v) const { return Mat3(m[0] / v, m[1] / v, m[2] / v); } //!< @brief Scalar division with matrix;

    //! @brief Matrix multiplication with a vector.
    Vec3 operator * (const Vec3& a) const
    {
        return Vec3(
            m[0][0] * a[0] + m[1][0] * a[1] + m[2][0] * a[2],
            m[0][1] * a[0] + m[1][1] * a[1] + m[2][1] * a[2],
            m[0][2] * a[0] + m[1][2] * a[1] + m[2][2] * a[2]);
    }

    //! @brief Matrix multiplication with another matrix.
    Mat3 operator * (const Mat3& a) const
    {
        return Mat3(
            m[0][0] * a[0][0] + m[1][0] * a[0][1] + m[2][0] * a[0][2], m[0][1] * a[0][0] + m[1][1] * a[0][1] + m[2][1] * a[0][2], m[0][2] * a[0][0] + m[1][2] * a[0][1] + m[2][2] * a[0][2],
            m[0][0] * a[1][0] + m[1][0] * a[1][1] + m[2][0] * a[1][2], m[0][1] * a[1][0] + m[1][1] * a[1][1] + m[2][1] * a[1][2], m[0][2] * a[1][0] + m[1][2] * a[1][1] + m[2][2] * a[1][2],
            m[0][0] * a[2][0] + m[1][0] * a[2][1] + m[2][0] * a[2][2], m[0][1] * a[2][0] + m[1][1] * a[2][1] + m[2][1] * a[2][2], m[0][2] * a[2][0] + m[1][2] * a[2][1] + m[2][2] * a[2][2]);

    }

private:

    ColumnType m[3];

};

//! @brief A 4x4 matrix in column major order comprised of Vec4.
class Mat4
{
public:

    using ColumnType = Vec4;

    //! @brief Default constructed which is uninitialized, identity matrix can be constructed using [Mat4(1.0f)](@ref Mat4(float)) constructor.
    Mat4() = default;

    //! @brief Initializes diagonal values with @p d and the rest with zero.
    explicit Mat4(float d) : Mat4(d,0,0,0, 0,d,0,0, 0,0,d,0, 0,0,0,d)
    {
    }

    //! @brief Expands a Mat3
    explicit Mat4(const Mat3& v) : m{ ColumnType(v[0], 0), ColumnType(v[1], 0), ColumnType(v[2], 0), ColumnType(0,0,0,1) }
    {
    }

    //! @brief Initializes matrix with columns as input.
    Mat4(const ColumnType& c0, const ColumnType& c1, const ColumnType& c2, const ColumnType& c3) : m{c0, c1, c2, c3}
    {
    }

    //! @brief Initializes matrix with each separate component.
    Mat4(float c00, float c01, float c02, float c03,
         float c10, float c11, float c12, float c13,
         float c20, float c21, float c22, float c23,
         float c30, float c31, float c32, float c33)
        : m{ ColumnType(c00, c01, c02, c03), ColumnType(c10, c11, c12, c13), ColumnType(c20, c21, c22, c23), ColumnType(c30, c31, c32, c33) }
    {
    }

    ColumnType&       operator [] (int i)       { return m[i]; } //!< @brief Access column of matrix, no bounds checking.
    const ColumnType& operator [] (int i) const { return m[i]; } //!< @brief Access column of matrix, no bounds checking.

    //! @brief Matrix multiplication with a vector.
    Vec4 operator * (const Vec4& a) const
    {
        return Vec4(
            m[0][0] * a[0] + m[1][0] * a[1] + m[2][0] * a[2] + m[3][0] * a[3],
            m[0][1] * a[0] + m[1][1] * a[1] + m[2][1] * a[2] + m[3][1] * a[3],
            m[0][2] * a[0] + m[1][2] * a[1] + m[2][2] * a[2] + m[3][2] * a[3],
            m[0][3] * a[0] + m[1][3] * a[1] + m[2][3] * a[2] + m[3][3] * a[3]);
    }

    //! @brief Matrix multiplication with another matrix.
    Mat4 operator * (const Mat4& a) const
    {
        return Mat4(
            m[0][0] * a[0][0] + m[1][0] * a[0][1] + m[2][0] * a[0][2] + m[3][0] * a[0][3], m[0][1] * a[0][0] + m[1][1] * a[0][1] + m[2][1] * a[0][2] + m[3][1] * a[0][3],
            m[0][2] * a[0][0] + m[1][2] * a[0][1] + m[2][2] * a[0][2] + m[3][2] * a[0][3], m[0][3] * a[0][0] + m[1][3] * a[0][1] + m[2][3] * a[0][2] + m[3][3] * a[0][3],

            m[0][0] * a[1][0] + m[1][0] * a[1][1] + m[2][0] * a[1][2] + m[3][0] * a[1][3], m[0][1] * a[1][0] + m[1][1] * a[1][1] + m[2][1] * a[1][2] + m[3][1] * a[1][3],
            m[0][2] * a[1][0] + m[1][2] * a[1][1] + m[2][2] * a[1][2] + m[3][2] * a[1][3], m[0][3] * a[1][0] + m[1][3] * a[1][1] + m[2][3] * a[1][2] + m[3][3] * a[1][3],

            m[0][0] * a[2][0] + m[1][0] * a[2][1] + m[2][0] * a[2][2] + m[3][0] * a[2][3], m[0][1] * a[2][0] + m[1][1] * a[2][1] + m[2][1] * a[2][2] + m[3][1] * a[2][3],
            m[0][2] * a[2][0] + m[1][2] * a[2][1] + m[2][2] * a[2][2] + m[3][2] * a[2][3], m[0][3] * a[2][0] + m[1][3] * a[2][1] + m[2][3] * a[2][2] + m[3][3] * a[2][3],

            m[0][0] * a[3][0] + m[1][0] * a[3][1] + m[2][0] * a[3][2] + m[3][0] * a[3][3], m[0][1] * a[3][0] + m[1][1] * a[3][1] + m[2][1] * a[3][2] + m[3][1] * a[3][3],
            m[0][2] * a[3][0] + m[1][2] * a[3][1] + m[2][2] * a[3][2] + m[3][2] * a[3][3], m[0][3] * a[3][0] + m[1][3] * a[3][1] + m[2][3] * a[3][2] + m[3][3] * a[3][3]);

    }


private:

    ColumnType m[4];

};



inline Mat3::Mat3(const Mat4& a) : m{ ColumnType(a[0]), ColumnType(a[1]), ColumnType(a[2]) }
{
}


}

