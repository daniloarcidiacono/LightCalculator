#pragma once
#include <vec3.h>

struct Triangle {
    vec3 a, b, c;

    Triangle() {
    }

    Triangle(const vec3 &a, const vec3 &b, const vec3 &c)
        : a(a), b(b), c(c) {
    }

    vec3 normal() const {
        return normalize(cross(b - a, c - a));
    }

    // Returns a uniformly distributed sample inside the triangle
    // r1, r2 uniform random numbers in [0; 1]
    // https://math.stackexchange.com/questions/18686/uniform-random-point-in-triangle-in-3d
    vec3 uniformSample(const float r1, const float r2) const {
        const float sr1 = sqrtf(r1);
        return a * (1 - sr1) +
               b * (sr1 * (1 - r2)) +
               c * (r2 * sr1);
    }

    // Uniform sampling area probability
    float uniformSamplePdf() const {
        return 1.0f / area();
    }

    float area() const {
        return 0.5f * length(cross(b - a, c - a));
    }
};

static std::ostream &operator <<(std::ostream &os, const Triangle &tri) {
    os << "{ a: " << tri.a << ", b: " << tri.b << ", c: " << tri.c << ", area: " << tri.area() << ", normal: " << tri.normal() << " }";
    return os;
}