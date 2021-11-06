#pragma once
#include <math.h>
#include <float.h>

constexpr float Pi = 3.14159265359f;
constexpr float To_Radians = 0.0174533f;
constexpr float Max_Float = 3.402823466e+38F;

#pragma pack(push, 1)

struct Vec2i {
    union {
        struct {int x, y; };
    };
};

struct Vec2 {
    union {
        struct {float x, y; };
    };

    Vec2() {}
    constexpr Vec2(float v) : x(v), y(v) {}
    constexpr Vec2(float x, float y) : x(x), y(y) {}
    Vec2(const Vec2i &other) {
        x = (float)other.x;
        y = (float)other.y;
    }

};

struct Vec3 {
    union {
        float array[3];
        struct { float x, y, z; };
        struct { Vec2 xy; float _z; };
    };

    Vec3() {}
    constexpr Vec3(float v) : x(v), y(v), z(v) {}
    constexpr Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    operator float* () const { return (float*)array; }
};

static _inline Vec3 operator+(const Vec3 &a, const Vec3 &b) {
    return Vec3 {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
    };
}

static _inline Vec3 operator-(const Vec3 &a, const Vec3 &b) {
    return Vec3 {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
    };
}

Vec3 operator*(const Vec3 &v, float t) {
    return Vec3 {
        v.x * t,
        v.y * t,
        v.z * t,
    };
}

Vec3 operator*(float t, const Vec3 &v) { return v * t; }

static _inline Vec3 operator/(const Vec3 &v, float t) {
    return Vec3 {
        v.x / t,
        v.y / t,
        v.z / t,
    };
}


struct Vec4 {
    union {
        struct { float x, y, z, w; };
        struct { Vec3 xyz; float __i0; };
        float array[4];
    };

    Vec4() {}
    constexpr Vec4(float v) : x(v), y(v), z(v), w(v) {}
    constexpr Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    float &operator[](int i) { return array[i]; }
    const float &operator[](int i) const { return array[i]; }
    operator float*() const { return (float*)array; }
};

struct Mat4 {
    union {
        float array[4][4];
        Vec4 rows[4];
        struct {
            float a0, a1, a2, a3;
            float b0, b1, b2, b3;
            float c0, c1, c2, c3;
            float d0, d1, d2, d3;
        };
    };

    Mat4() {}
    Mat4(float v) :Mat4(v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v) {} // hehe
    Mat4(
        float _a0, float _a1, float _a2, float _a3,
        float _b0, float _b1, float _b2, float _b3,
        float _c0, float _c1, float _c2, float _c3,
        float _d0, float _d1, float _d2, float _d3) {

        a0 = _a0; a1 = _a1; a2 = _a2; a3 = _a3;
        b0 = _b0; b1 = _b1; b2 = _b2; b3 = _b3;
        c0 = _c0; c1 = _c1; c2 = _c2; c3 = _c3;
        d0 = _d0; d1 = _d1; d2 = _d2; d3 = _d3;
    }

    Vec4 &operator[](int i) { return rows[i]; }
    const Vec4 &operator[](int i) const { return rows[i]; }

    // operator
    operator float*() const { return (float*)array; }
};

#pragma pack(pop)

static _inline float dot(const Vec2 &a, const Vec2 &b) {return (a.x * b.x + a.y * b.y);}
static _inline float dot(const Vec3 &a, const Vec3 &b) {return (a.x * b.x + a.y * b.y + a.z * b.z);}
static _inline float dot(const Vec4 &a, const Vec4 &b) {return (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);}

static _inline float lensq(const Vec3 &v) {
    float l = dot(v, v);
    return l;
}

static Vec3 normalize(const Vec3 &a) {
    float l = lensq(a);

    if (l == 0.f) return Vec3(0);

    return a / sqrtf(l);
}

static _inline Mat4 operator *(const Mat4 &left, const Mat4 &right) {
    Mat4 result;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            result[r][c] =
                left[r].x * right[0][c] +
                left[r].y * right[1][c] +
                left[r].z * right[2][c] +
                left[r].w * right[3][c];
        }
    }
    return result;
}

static _inline Mat4 transpose(const Mat4 &m) {
    return Mat4 {
        m.rows[0].x, m.rows[1].x, m.rows[2].x, m.rows[3].x,
        m.rows[0].y, m.rows[1].y, m.rows[2].y, m.rows[3].y,
        m.rows[0].z, m.rows[1].z, m.rows[2].z, m.rows[3].z,
        m.rows[0].w, m.rows[1].w, m.rows[2].w, m.rows[3].w
    };
}

static _inline Mat4 translation(Vec3 v) {
    return Mat4 {
        1, 0, 0, v.x,
        0, 1, 0, v.y,
        0, 0, 1, v.z,
        0, 0, 0, 1
    };
}

static _inline Mat4 rotation_pitch(float angle) {
    return Mat4 {
        1,  0,            0,            0,
        0,  cosf(angle),  -sinf(angle), 0,
        0,  sinf(angle),  cosf(angle),  0,
        0,  0,            0,            1,
    };
}

static _inline Mat4 rotation_yaw(float angle) {
    return Mat4 {
        cosf(angle),  0,  sinf(angle), 0,
        0,            1,  0,           0,
        -sinf(angle), 0,  cosf(angle), 0,
        0,            0,  0,           1
    };
}

static _inline Mat4 scale(Vec3 by) {
    return Mat4 {
        by.x, 0,    0,    0,
        0,    by.y, 0,    0,
        0,    0,    by.z, 0,
        0,    0,    0,    1,
    };
}

static _inline Mat4 perspective(float fov, float aspect, float znear, float zfar) {
    const float th = tanf(fov / 2.f);
    return  Mat4 {
        1.f / (aspect * th), 0,        0,                                 0,
        0,                   1.f / th, 0,                                 0,
        0,                   0,        - (zfar + znear) / (zfar - znear), -2.f * (zfar * znear) / (zfar - znear),
        0,                   0,        1.f,                               0

    };
}

template <typename T>
static _inline T min(T a, T b) { return a < b ? a : b; }
template <typename T>
static _inline T max(T a, T b) { return a > b ? a : b; }

template <typename T>
static _inline T clamp(T x, T a, T b) {
    return max(a, min(x, b));
}

static _inline Vec3 operator*(const Mat4 &m, const Vec3 &v) {
    Vec4 v4;
    v4.xyz = v;
    v4.w = 0;

    return Vec3 {
        dot(v4, m[0]),
        dot(v4, m[1]),
        dot(v4, m[2]),
    };
}