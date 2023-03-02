#pragma once
#include "Base.h"

/**
 * Vectors
 */
#pragma pack(push, 1)

struct MOKLIB_API Vec2i {
    union {
        struct {
            int x, y;
        };
    };

    Vec2i() = default;
    constexpr Vec2i(int x, int y) : x(x), y(y) {}
};

static _inline bool operator==(const Vec2i& left, const Vec2i& right)
{
    return (left.x == right.x) && (left.y == right.y);
}

static _inline Vec2i operator-(const Vec2i& left, const Vec2i& right)
{
    return Vec2i{(left.x - right.x), (left.y - right.y)};
}

struct Vec2 {
    union {
        struct {
            float x, y;
        };
    };

    Vec2() {}
    constexpr Vec2(float v) : x(v), y(v) {}
    constexpr Vec2(float x, float y) : x(x), y(y) {}
    Vec2(const Vec2i& other)
    {
        x = (float)other.x;
        y = (float)other.y;
    }

    Vec2 operator-() const { return Vec2{-x, -y}; }

    Vec2 operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
};

static _inline Vec2 operator+(const Vec2& a, const Vec2& b)
{
    return Vec2{
        a.x + b.x,
        a.y + b.y,
    };
}

static _inline Vec2 operator-(const Vec2& a, const Vec2& b)
{
    return Vec2{
        a.x - b.x,
        a.y - b.y,
    };
}

static _inline Vec2 operator*(const Vec2& v, float t)
{
    return Vec2{
        v.x * t,
        v.y * t,
    };
}

struct MOKLIB_API Vec3 {
    union {
        struct {
            float x, y, z;
        };
        float array[3];
    };

    Vec3() : x(0), y(0), z(0) {}
    constexpr Vec3(float v) : x(v), y(v), z(v) {}
    constexpr Vec3(const Vec2& xy, float z = 0.f) : x(xy.x), y(xy.y), z(z) {}
    constexpr Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    operator float*() const { return (float*)array; }

    static constexpr Vec3 right() { return Vec3(1, 0, 0); }
    static constexpr Vec3 up() { return Vec3(0, 1, 0); }
    static constexpr Vec3 forward() { return Vec3(0, 0, 1); }
};

static _inline Vec3 operator+(const Vec3& a, const Vec3& b)
{
    return Vec3{
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
    };
}

static _inline Vec3 operator-(const Vec3& a, const Vec3& b)
{
    return Vec3{
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
    };
}

static _inline Vec3 operator*(const Vec3& v, float t)
{
    return Vec3{
        v.x * t,
        v.y * t,
        v.z * t,
    };
}

static _inline Vec3 operator*(float t, const Vec3& v) { return v * t; }

static _inline Vec3 operator/(const Vec3& v, float t)
{
    return Vec3{
        v.x / t,
        v.y / t,
        v.z / t,
    };
}

struct Vec4 {
    union {
        float array[4];
        struct {
            float x, y, z, w;
        };
    };

    Vec4() {}
    Vec4(Vec3 v3, float vw)
    {
        x = v3.x;
        y = v3.y;
        z = v3.z;
        w = vw;
    }

    Vec2& xy() const { return *((Vec2*)array); }

    Vec2& zw() const { return *((Vec2*)(array + 2)); }

    Vec3& xyz() const { return *((Vec3*)array); }

    constexpr Vec4(float v) : x(v), y(v), z(v), w(v) {}
    constexpr Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
    {}

    float&       operator[](int i) { return array[i]; }
    const float& operator[](int i) const { return array[i]; }
                 operator float*() const { return (float*)array; }
};

struct Mat4 {
    union {
        struct {
            float a0, a1, a2, a3;
            float b0, b1, b2, b3;
            float c0, c1, c2, c3;
            float d0, d1, d2, d3;
        };
        float array[4][4];
        Vec4  rows[4];
    };

    Mat4() {}
    constexpr Mat4(float v)
        : Mat4(v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v)
    {}  // hehe
    constexpr Mat4(
        float a0,
        float a1,
        float a2,
        float a3,
        float b0,
        float b1,
        float b2,
        float b3,
        float c0,
        float c1,
        float c2,
        float c3,
        float d0,
        float d1,
        float d2,
        float d3)
        : a0(a0)
        , a1(a1)
        , a2(a2)
        , a3(a3)
        , b0(b0)
        , b1(b1)
        , b2(b2)
        , b3(b3)
        , c0(c0)
        , c1(c1)
        , c2(c2)
        , c3(c3)
        , d0(d0)
        , d1(d1)
        , d2(d2)
        , d3(d3)
    {}

    static constexpr Mat4 identity()
    {
        return Mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    }

    Vec4&       operator[](int i) { return rows[i]; }
    const Vec4& operator[](int i) const { return rows[i]; }

    // operator
    operator float*() const { return (float*)array; }
};

#pragma pack(pop)

static _inline Vec3 scale(const Vec3& a, const Vec3& b)
{
    return Vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

static _inline Vec3 scale(const Vec3& a, float b) { return scale(a, Vec3(b)); }

static _inline float dot(const Vec2& a, const Vec2& b)
{
    return (a.x * b.x + a.y * b.y);
}
static _inline float dot(const Vec3& a, const Vec3& b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}
static _inline float dot(const Vec4& a, const Vec4& b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
}

static _inline Vec3 cross(const Vec3& a, const Vec3& b)
{
    return Vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

static _inline float lensq(const Vec3& v)
{
    float l = dot(v, v);
    return l;
}

static Vec3 normalize(const Vec3& a)
{
    float l = lensq(a);

    if (l == 0.f) return Vec3(0);

    return a / sqrtf(l);
}

static _inline Mat4 operator*(const Mat4& left, const Mat4& right)
{
    Mat4 result(0.f);
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            for (int k = 0; k < 4; ++k) {
                result[r][c] += left[r][k] * right[k][c];
            }
        }
    }
    return result;
}

static _inline Mat4 transpose(const Mat4& m)
{
    return Mat4{
        m.rows[0].x,
        m.rows[1].x,
        m.rows[2].x,
        m.rows[3].x,
        m.rows[0].y,
        m.rows[1].y,
        m.rows[2].y,
        m.rows[3].y,
        m.rows[0].z,
        m.rows[1].z,
        m.rows[2].z,
        m.rows[3].z,
        m.rows[0].w,
        m.rows[1].w,
        m.rows[2].w,
        m.rows[3].w};
}

static _inline Mat4 translation(Vec3 v)
{
    Mat4 result = Mat4::identity();
    // result[3][0] = v.x;
    // result[3][1] = v.y;
    // result[3][2] = v.z;

    result = Mat4{
        // Row 0
        1,
        0,
        0,
        0,
        // Row 1
        0,
        1,
        0,
        0,
        // Row 2
        0,
        0,
        1,
        0,
        // Row 3
        v.x,
        v.y,
        v.z,
        1,
    };
    return result;
}

static _inline Mat4 rotation_pitch(float angle)
{
    // clang-format off
    return Mat4{
        1, 0, 0, 0,
        0, cosf(angle), sinf(angle), 0,
        0, -sinf(angle), cosf(angle), 0,
        0, 0, 0, 1,
    };

    // clang-format on
}

static _inline Mat4 rotation_yaw(float angle)
{
    // clang-format off
    return Mat4{
        cosf(angle), 0, -sinf(angle), 0,
        0, 1, 0, 0,
        sinf(angle), 0, cosf(angle), 0,
        0, 0, 0, 1,
    };
    // clang-format on
}

static _inline Mat4 rotation(Vec3 u, float a)
{
    const float ct = cosf(a);
    const float st = sinf(a);
    return Mat4{
        ct + sq(u.x) * (1.f - ct),
        u.x * u.y * (1.f - ct) - u.z * st,
        u.x * u.z * (1.f - ct) + u.y * st,
        0.f,
        u.y * u.x * (1.f - ct) + u.z * st,
        ct + sq(u.y) * (1.f - ct),
        u.y * u.z * (1.f - ct) - u.x * st,
        0.f,
        u.z * u.x * (1.f - ct) - u.y * st,
        u.z * u.y * (1.f - ct) + u.x * st,
        ct + sq(u.z) * (1.f - ct),
        0.f,
        0.f,
        0.f,
        0.f,
        1.f};
}

static _inline Mat4 scale(Vec3 by)
{
    return Mat4{
        by.x,
        0,
        0,
        0,
        0,
        by.y,
        0,
        0,
        0,
        0,
        by.z,
        0,
        0,
        0,
        0,
        1,
    };
}

static _inline Mat4 lookat(Vec3 forward, Vec3 up, Vec3 eye)
{
    Vec3 right = normalize(cross(up, forward));

    return Mat4{
        right.x,
        right.y,
        right.z,
        0.f,
        up.x,
        up.y,
        up.z,
        0.f,
        forward.x,
        forward.y,
        forward.z,
        0.f,
        -eye.x,
        -eye.y,
        -eye.z,
        1.f};
}

static _inline Mat4 orthographic(
    float b, float t, float l, float r, float n, float f)
{
    return Mat4{
        2.f / (r - l),
        0,
        0,
        -(r + l) / (r - l),
        0,
        2.f / (t - b),
        0,
        -(t + b) / (t - b),
        0,
        0,
        -2.f / (f - n),
        -(f + n) / (f - n),
        0,
        0,
        0,
        1.f,
    };
}

static _inline Mat4 perspective(
    float fov, float aspect, float znear, float zfar)
{
    const float focal_length = 1.0f / tanf(fov / 2.0f);
    const float clip_range   = znear - zfar;

    return Mat4{
        // Row 0
        focal_length / aspect,
        0,
        0,
        0,
        // Row 1
        0,
        -focal_length,
        0,
        0,
        // Row 2
        0,
        0,
        zfar / (znear - zfar),
        -1,
        // Row 3
        0,
        0,
        (zfar * znear) / (znear - zfar),
        0,
    };
}

static _inline Vec3 operator*(const Mat4& m, const Vec3& v)
{
    Vec4 v4(v, 0);

    return Vec3{
        dot(v4, m[0]),
        dot(v4, m[1]),
        dot(v4, m[2]),
    };
}

static _inline Vec2i absolute(const Vec2i& v)
{
    return Vec2i{absolute(v.x), absolute(v.y)};
}

static _inline f32 distance(const Vec2i& a, const Vec2i& b)
{
    Vec2i diff = (b - a);
    return sqrtf(f32(diff.x * diff.x) + f32(diff.y * diff.y));
}

struct Quat {
    Quat() : v(0) {}

    Quat(const Vec3& axis, float angle)
    {
        const float s = sinf(angle * .5f);
        v.xyz()       = scale(v.xyz(), s);
    }

    union {
        struct {
            float x, y, z, w;
        };
        Vec4 v;
    };
};

static _inline Quat operator*(const Quat& q1, const Quat& q2)
{
    Quat result;
    result.x = q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
    result.y = -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;
    result.z = q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
    result.w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;
    return result;
}

static _inline Vec3 operator*(const Quat& q, const Vec3& v)
{
    Vec3  u = q.v.xyz();
    float s = q.w;

    Vec3 result = scale(u, 2 * dot(u, v));
    result      = result + scale(v, s * s - dot(u, u));
    result      = result + scale(cross(u, v), 2 * s);
    return result;
}

template <>
Vec3 max(Vec3 a, Vec3 b)
{
    return Vec3{max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)};
}

template <>
Vec3 min(Vec3 a, Vec3 b)
{
    return Vec3{min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)};
}

// template <typename T, int D>
// struct TVec {
//     T array[D];

//     template <int D2>
//     TVec(const TVec<T, D2> &other) {
//         constexpr int n = min(D, D2);

//         for (int i = 0; i < n; ++i) {
//             array[]
//         }
//     }
// };