
#pragma once

#include "matrix.hpp"

#include <tuple>

namespace Math
{

inline Mat3 Transpose(const Mat3& m)
{
    return Mat3(
        m[0][0], m[1][0], m[2][0],
        m[0][1], m[1][1], m[2][1],
        m[0][2], m[1][2], m[2][2]);
}

inline Mat4 Transpose(const Mat4& m)
{
    return Mat4(
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]);
}

//! @brief Creates a 4x4 perspective matrix and its' inverse.
//! @param [in] fov   The vertical field of view in radians.
//! @param [in] ratio The ratio between width and height, (width / height).
//! @param [in] zNear The z near distance of the frustrum, should not be zero.
//! @param [in] zFar  The z far distance of the frustrum.
//! @returns A std::pair<Mat4, Mat4> where first is the perspective matrix and second is its' inverse.
inline std::pair<Mat4, Mat4> PerspectiveMatrixAndInverse(float fov, float ratio, float zNear, float zFar)
{
    float f = 1.0f / std::tan(fov / 2);
    float d = zNear - zFar;

    std::pair<Mat4, Mat4> matrices = std::make_pair(Mat4(0.0f), Mat4(0.0f));
    
    matrices.first[0][0] = f / ratio;
    matrices.first[1][1] = f;
    matrices.first[2][2] = (zNear + zFar) / d;
    matrices.first[2][3] = -1.0f;
    matrices.first[3][2] = (2 * zNear * zFar) / d;

    // inverse

    matrices.second[0][0] = ratio / f;
    matrices.second[1][1] = 1.0f / f;
    matrices.second[2][3] = d / (2 * zNear * zFar);
    matrices.second[3][2] = -1.0f;
    matrices.second[3][3] = (zNear + zFar) / (2 * zNear * zFar);

    return matrices;
}

inline Mat4 Ortho(float left, float right, float bot, float top)
{
    Mat4 matrix(1.0f);

    matrix[0][0] = 2.0f / (right - left);
    matrix[1][1] = 2.0f / (top - bot);
    matrix[2][2] = -1.0f;
    matrix[3][0] = -(right + left) / (right - left);
    matrix[3][1] = -(top + bot) / (top - bot);

    return matrix;
}

inline Mat3 AxisAngleMatrix(const Vec3& axis, float radians)
{
    float c = std::cos(radians);
    float s = std::sin(radians);
    float t = 1.0f - c;

    const Vec3& n = axis;

    return Mat3(
        t * n.x * n.x + c,       t * n.x * n.y + n.z * s, t * n.x * n.z - n.y * s,
        t * n.x * n.y - n.z * s, t * n.y * n.y + c,       t * n.y * n.z + n.x * s,
        t * n.x * n.z + n.y * s, t * n.y * n.z - n.x * s, t * n.z * n.z + c);

}

inline Mat3 EulerAngleX(float radians)
{
    float s = std::sin(radians);
    float c = std::cos(radians);

    return Mat3(
        1,  0,  0,
        0,  c,  s,
        0, -s,  c);
}

inline Mat3 EulerAngleY(float radians)
{
    float s = std::sin(radians);
    float c = std::cos(radians);

    return Mat3(
        c,  0, -s,
        0,  1,  0,
        s,  0,  c);
}

inline Mat3 EulerAngleZ(float radians)
{
    float s = std::sin(radians);
    float c = std::cos(radians);

    return Mat3(
        c,  s,  0,
        -s, c,  0,
        0,  0,  1);
}

}