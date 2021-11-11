#pragma once
#include <ostream>

struct vec3 {
    float x, y, z;

    vec3() {
        x = y = z = 0;
    }

    vec3(float x, float y, float z)
        : x(x), y(y), z(z) {
    }

    // Vectors
    vec3 operator +(const vec3 &other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    vec3 operator -(const vec3 &other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    vec3 operator *(const vec3 &other) const {
        return {x * other.x, y * other.y, z * other.z};
    }

    vec3 operator /(const vec3 &other) const {
        return {x / other.x, y / other.y, z / other.z};
    }

    vec3 &operator +=(const vec3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    vec3 &operator -=(const vec3 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    vec3 &operator *=(const vec3 &other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    vec3 &operator /=(const vec3 &other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    // Scalars
    vec3 operator +(const float scalar) const {
        return { x + scalar, y + scalar, z + scalar };
    }

    vec3 operator -(const float scalar) const {
        return { x - scalar, y - scalar, z - scalar };
    }

    vec3 operator *(const float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }

    vec3 operator /(const float scalar) const {
        return { x / scalar, y / scalar, z / scalar };
    }

    vec3 &operator +=(const float scalar) {
        x += scalar;
        y += scalar;
        z += scalar;
        return *this;
    }

    vec3 &operator -=(const float scalar) {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        return *this;
    }

    vec3 &operator *=(const float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    vec3 &operator /=(const float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    // Other operations
    friend float lengthSquared(const vec3 &v) {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }

    friend float length(const vec3 &v) {
        return sqrt(lengthSquared(v));
    }

    friend float dot(const vec3 &lhs, const vec3 &rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    friend vec3 cross(const vec3 &a, const vec3 &b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    friend vec3 normalize(const vec3 &v) {
        const float l = length(v);
        return l != 0 ? (v / l) : v;
    }
};

static std::ostream &operator <<(std::ostream &os, const vec3 &v) {
    os << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
    return os;
}