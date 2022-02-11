#pragma once
#include "Base.h"
#include "../Base.h"

struct Recti {
    Vec2i pos;
    Vec2i size;

    Recti() : Recti(0,0,0,0) {}
    Recti(i32 x, i32 y, i32 w, i32 h) {
        pos = {x, y};
        size = {w ,h};
    }

    _inline Vec2i top_left()  { return { pos.x, pos.y }; }
    _inline Vec2i top_right() { return { pos.x + size.x, pos.y }; }
    _inline Vec2i bot_left()  { return { pos.x, pos.y + size.y }; }
    _inline Vec2i bot_right() { return { pos.x + size.x, pos.y + size.y }; }

};

struct Rectf {
    Vec2 pos;
    Vec2 size;

    Rectf() : Rectf(0,0,0,0) {}
    Rectf(f32 x, f32 y, f32 w, f32 h) {
        pos = {x, y};
        size = {w ,h};
    }

    _inline Vec2 top_left()  { return { pos.x, pos.y }; }
    _inline Vec2 top_right() { return { pos.x + size.x, pos.y }; }
    _inline Vec2 bot_left()  { return { pos.x, pos.y + size.y }; }
    _inline Vec2 bot_right() { return { pos.x + size.x, pos.y + size.y }; }
};

struct Sphere {
    Vec3 center;
    float radius;
};

struct Ray {
    Vec3 origin;
    Vec3 dir;
};

bool sphere_vs_ray(const Ray &ray, const Sphere &sphere, float &t0) {
    Vec3 l = sphere.center - ray.origin;
    float tca = dot(l, ray.dir);
    float d2 = dot(l, l) - tca * tca;

    if (d2 > sphere.radius) {
        return false;
    }

    float thc = sqrtf(sphere.radius*sphere.radius - d2);
    t0 = tca - thc;
    float t1 = tca + thc;
    if (t0 < 0) t0 = t1;
    if (t0 < 0) return false;
    
    return true;
}