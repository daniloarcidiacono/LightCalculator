#pragma once
#include <math/vec3.h>
#include <ostream>

struct Light {
    vec3 position;
    vec3 color;
    float intensity;

    Light() {
        intensity = 1;
    }

    Light(const vec3 &position, const vec3 &color, const float intensity)
        : position(position), color(color), intensity(intensity) {
    }
};

static std::ostream &operator <<(std::ostream &os, const Light &light) {
    os << "{ position: " << light.position << ", intensity: " << light.intensity << " }";
    return os;
}
