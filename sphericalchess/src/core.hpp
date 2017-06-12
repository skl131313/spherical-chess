
#pragma once

#include "math/math.hpp"
#include "util.hpp"

#include <cstdint>

using namespace Math::LiteralOperators;


using Vec2  = Math::Vec2;
using Vec2i = Math::Vec2i;

using Vec3 = Math::Vec3;
using Vec4 = Math::Vec4;

using Plane3 = Math::Plane3;

using Mat3 = Math::Mat3;
using Mat4 = Math::Mat4;

using Rect2  = Math::Rect2;
using Rect2i = Math::Rect2i;

using f32 = float;
using f64 = double;

using s8  = int8_t;     static_assert(sizeof(s8)  == 1, "s8  is not 8  bits");
using s16 = int16_t;    static_assert(sizeof(s16) == 2, "s16 is not 16 bits");
using s32 = int32_t;    static_assert(sizeof(s32) == 4, "s32 is not 32 bits");
using s64 = int64_t;    static_assert(sizeof(s64) == 8, "s64 is not 64 bits");

using u8  = uint8_t;    static_assert(sizeof(u8)  == 1, "u8  is not 8  bits");
using u16 = uint16_t;   static_assert(sizeof(u16) == 2, "u16 is not 16 bits");
using u32 = uint32_t;   static_assert(sizeof(u32) == 4, "u32 is not 32 bits");
using u64 = uint64_t;   static_assert(sizeof(u64) == 8, "u64 is not 64 bits");