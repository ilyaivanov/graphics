#pragma once
#include "types.h"
#include "xmmintrin.h"

// Floats

inline f32 Clamp(f32 val, f32 min, f32 max)
{
    if (val > max)
        return max;
    if (val < min)
        return min;
    return val;
}

inline float Sqrt(const float val)
{
    return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(val)));
}

inline i32 Round(f32 val)
{
    return (i32)(val + 0.5f);
}

inline i32 AbsInt(i32 val)
{
    if (val < 0)
        return -val;
    return val;
}
inline f32 Absf32(f32 val)
{
    if (val < 0)
        return -val;
    return val;
}

inline i32 Min4Int(i32 a, i32 b, i32 c, i32 d)
{
    if (a <= b && a <= c && a <= d)
        return a;
    if (b <= a && b <= c && b <= d)
        return b;
    if (c <= a && c <= b && c <= d)
        return c;
    return d;
}
inline i32 Max4Int(i32 a, i32 b, i32 c, i32 d)
{
    if (a >= b && a >= c && a >= d)
        return a;
    if (b >= a && b >= c && b >= d)
        return b;
    if (c >= a && c >= b && c >= d)
        return c;
    return d;
}

inline i32 Min3Int(i32 a, i32 b, i32 c)
{
    if (a <= b && a <= c)
        return a;
    if (b <= a && b <= c)
        return b;
    return c;
}
inline i32 Max3Int(i32 a, i32 b, i32 c)
{
    if (a >= b && a >= c)
        return a;
    if (b >= a && b >= c)
        return b;
    return c;
}
inline i32 Min2Int(i32 a, i32 b)
{
    if (a <= b)
        return a;
    return b;
}
inline i32 Max2Int(i32 a, i32 b)
{
    if (a >= b)
        return a;
    return b;
}

inline f32 Min2F(f32 a, f32 b)
{
    if (a <= b)
        return a;
    return b;
}

//
// Vectors
//
//

// clang-format off

typedef struct V2f { f32 x, y;    } V2f;
typedef struct V2i { i32 x, y;    } V2i;
typedef struct V3f { f32 x, y, z; } V3f;
typedef struct V4f { f32 x, y, z, w; } V4f;

// clang-format on

f32 V2fDistance(V2f point1, V2f point2)
{
    f32 dx = point1.x - point2.x;
    f32 dy = point1.y - point2.y;
    return Sqrt(dx * dx + dy * dy);
}

inline V2f V2fMult(V2f p, f32 scalar)
{
    return (V2f){p.x * scalar, p.y * scalar};
}

inline V2f V2fAdd(V2f p1, V2f p2)
{
    return (V2f){p1.x + p2.x, p1.y + p2.y};
}

inline V2f V2fAddScalar(V2f p1, f32 scalar)
{
    return (V2f){p1.x + scalar, p1.y + scalar};
}

inline V2f V2fDiffScalar(V2f p1, f32 scalar)
{
    return (V2f){p1.x - scalar, p1.y - scalar};
}

inline V2f V2fDiff(V2f p1, V2f p2)
{
    return (V2f){p1.x - p2.x, p1.y - p2.y};
}

inline V2f V2fNormalize(V2f v)
{
    float magnitude = Sqrt(v.x * v.x + v.y * v.y);

    if (magnitude == 0.0f)
        return v;
    else
        return (V2f){v.x / magnitude, v.y / magnitude};
}

inline V3f V3fLerpXY(V3f v, f32 minX, f32 maxX, f32 minY, f32 maxY)
{
    if (v.x < minX)
        v.x = minX;
    if (v.x > maxX)
        v.x = maxX;

    if (v.y < minY)
        v.y = minY;
    if (v.y > maxY)
        v.y = maxY;

    return v;
}

inline V3f V3fAdd3(V3f p1, V3f p2, V3f p3)
{
    return (V3f){p1.x + p2.x + p3.x, p1.y + p2.y + p3.y, p1.z + p2.z + p3.z};
}

inline V3f V3fMul(V3f p1, f32 scalar)
{
    return (V3f){p1.x * scalar, p1.y * scalar, p1.z * scalar};
}

inline f32 Lerp3f(f32 p1, f32 p2, f32 p3, f32 t1, f32 t2, f32 t3)
{
    return p1 * t1 + p2 * t2 + p3 * t3;
}

inline V3f LerpV3f(V3f p1, V3f p2, V3f p3, f32 t1, f32 t2, f32 t3)
{
    return V3fAdd3(V3fMul(p1, t1), V3fMul(p2, t2), V3fMul(p3, t3));
}

inline f32 Lerp3u(u32 p1, u32 p2, u32 p3, f32 t1, f32 t2, f32 t3)
{
    return p1 * t1 + p2 * t2 + p3 * t3;
}

float V2fCross(V2f p1, V2f p2)
{
    return (p1.x * p2.y) - (p1.y * p2.x);
}

inline V4f V4(f32 x, f32 y, f32 z)
{
    return (V4f){x, y, z, 1};
}
inline V3f V3(f32 x, f32 y, f32 z)
{
    return (V3f){x, y, z};
}

inline V4f V4Direction(f32 x, f32 y, f32 z)
{
    return (V4f){x, y, z, 0};
}

//
// Matrixes
//
//

typedef struct Mat4
{
    float values[16];
} Mat4;

#define M4_AT(mat, x, y) mat.values[x + y * 4]

inline Mat4 Mat4Identity()
{
    // clang-format off
    return (Mat4){
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
    // clang-format on
}

inline Mat4 Mat4Mult(Mat4 m1, Mat4 m2)
{
    Mat4 result = {0};
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            for (int k = 0; k < 4; k++)
                M4_AT(result, col, row) += M4_AT(m1, k, row) * M4_AT(m2, col, k);

    return result;
}
inline V4f Mat4MultV4f(Mat4 m, V4f v)
{
    V4f res;
    res.x = M4_AT(m, 0, 0) * v.x + M4_AT(m, 1, 0) * v.y + M4_AT(m, 2, 0) * v.z + M4_AT(m, 3, 0) * v.w;
    res.y = M4_AT(m, 0, 1) * v.x + M4_AT(m, 1, 1) * v.y + M4_AT(m, 2, 1) * v.z + M4_AT(m, 3, 1) * v.w;
    res.z = M4_AT(m, 0, 2) * v.x + M4_AT(m, 1, 2) * v.y + M4_AT(m, 2, 2) * v.z + M4_AT(m, 3, 2) * v.w;
    res.w = M4_AT(m, 0, 3) * v.x + M4_AT(m, 1, 3) * v.y + M4_AT(m, 2, 3) * v.z + M4_AT(m, 3, 3) * v.w;
    return res;
}

inline Mat4 CreateScreenProjection(V2i screen)
{
    // allows me to set vecrtex coords as 0..width/height, instead of -1..+1
    // 0,0 is bottom left, not top left
    // matrix in code != matrix in math notation, details at https://youtu.be/kBuaCqaCYwE?t=3084
    // in short: rows in math are columns in code
    // clang-format off
    return (Mat4){
        2.0f / (f32)screen.x, 0,                    0, -1,
        0,                    2.0f / (f32)screen.y, 0, -1,
        0,                    0,                    1,  0,
        0,                    0,                    0,  1,
    };
    // clang-format on
}

inline Mat4 Mat4TranslateV3f(Mat4 mat, V3f v)
{
    mat.values[3 + 0 * 4] += v.x;
    mat.values[3 + 1 * 4] += v.y;
    mat.values[3 + 2 * 4] += v.z;
    return mat;
}

inline Mat4 Mat4TranslateV2f(Mat4 mat, V2f v)
{
    mat.values[3 + 0 * 4] += v.x;
    mat.values[3 + 1 * 4] += v.y;
    return mat;
}

inline Mat4 Mat4TranslateXY(Mat4 mat, f32 x, f32 y)
{
    mat.values[3 + 0 * 4] += x;
    mat.values[3 + 1 * 4] += y;
    return mat;
}
inline Mat4 Mat4TranslateXYZ(f32 x, f32 y, f32 z)
{
    Mat4 mat = Mat4Identity();
    mat.values[3 + 0 * 4] += x;
    mat.values[3 + 1 * 4] += y;
    mat.values[3 + 2 * 4] += z;
    return mat;
}

inline Mat4 Mat4TranslateZ(Mat4 mat, f32 z)
{
    M4_AT(mat, 3, 2) = z;
    return mat;
}

inline Mat4 Mat4Scale1f(Mat4 mat, float v)
{
    mat.values[0 + 0 * 4] *= v;
    mat.values[1 + 1 * 4] *= v;
    mat.values[2 + 2 * 4] *= v;
    return mat;
}

inline Mat4 Mat4ScaleV3f(Mat4 mat, V3f v)
{
    mat.values[0 + 0 * 4] *= v.x;
    mat.values[1 + 1 * 4] *= v.y;
    mat.values[2 + 2 * 4] *= v.z;
    return mat;
}

inline Mat4 Mat4ScaleUniform(Mat4 mat, f32 scalar)
{
    mat.values[0 + 0 * 4] *= scalar;
    mat.values[1 + 1 * 4] *= scalar;
    mat.values[2 + 2 * 4] *= scalar;
    return mat;
}

inline Mat4 Mat4ScaleXY(Mat4 mat, f32 x, f32 y)
{
    mat.values[0 + 0 * 4] *= x;
    mat.values[1 + 1 * 4] *= y;
    return mat;
}

inline Mat4 Mat4RotateZ(f32 rads)
{
    Mat4 mat = Mat4Identity();
    f32 sin;
    f32 cos;
    MySinCos(rads, &sin, &cos);

    M4_AT(mat, 0, 0) = cos;
    M4_AT(mat, 0, 1) = -sin;
    M4_AT(mat, 1, 0) = sin;
    M4_AT(mat, 1, 1) = cos;
    return mat;
}

inline Mat4 Mat4RotateXYZ(f32 x, f32 y, f32 z)
{
    f32 sin;
    f32 cos;

    Mat4 matX = Mat4Identity();
    MySinCos(x, &sin, &cos);
    M4_AT(matX, 1, 1) = cos;
    M4_AT(matX, 1, 2) = -sin;
    M4_AT(matX, 2, 1) = sin;
    M4_AT(matX, 2, 2) = cos;

    Mat4 matY = Mat4Identity();
    MySinCos(y, &sin, &cos);
    M4_AT(matY, 0, 0) = cos;
    M4_AT(matY, 0, 2) = -sin;
    M4_AT(matY, 2, 0) = sin;
    M4_AT(matY, 2, 2) = cos;

    Mat4 matZ = Mat4Identity();
    MySinCos(z, &sin, &cos);
    M4_AT(matZ, 0, 0) = cos;
    M4_AT(matZ, 0, 1) = -sin;
    M4_AT(matZ, 1, 0) = sin;
    M4_AT(matZ, 1, 1) = cos;

    Mat4 mat = Mat4Mult(matX, Mat4Mult(matY, matZ));
    return mat;
}

inline Mat4 Mat4RotateY(f32 rads)
{
    Mat4 mat = Mat4Identity();
    f32 sin;
    f32 cos;
    MySinCos(rads, &sin, &cos);
    M4_AT(mat, 0, 0) = cos;
    M4_AT(mat, 0, 2) = -sin;
    M4_AT(mat, 2, 0) = sin;
    M4_AT(mat, 2, 2) = cos;
    return mat;
}
