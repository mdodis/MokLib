#pragma once
#include "Base.h"

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