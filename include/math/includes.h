#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

static constexpr float clamp(const float value, const float min, const float max, const float epsilon = 0) {
    return std::min(max - epsilon, std::max(min + epsilon, value));
}

static constexpr long double operator ""_deg(long double value) {
    return value * M_PI / 180;
}

static constexpr float operator ""_rgbf(unsigned long long value) {
    return value / 255.0f;
}
