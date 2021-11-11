#pragma once
#include <iostream>

class vec3 {
public:
	union {
		struct { float x, y, z; };
		struct { float r, g, b; };
		struct { float u, v, w; };
		float values[3];
	};

	vec3(const vec3& a) {
		x = a.x;
		y = a.y;
		z = a.z;
	}

	vec3(const float s = 0) {
		x = y = z = s;
	}

	vec3(const float values[3]) {
		x = values[0];
		y = values[1];
		z = values[2];
	}

	vec3(const float x, const float y, const float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	static vec3 lerp(const vec3& a, const vec3& b, const float u) {
		return vec3(
			a.x + (b.x - a.x) * u,
			a.y + (b.y - a.y) * u,
			a.z + (b.z - a.z) * u
		);
	}

	vec3 cross(const vec3& v) const {
		return vec3(
			values[1] * v[2] - values[2] * v[1],
			values[2] * v[0] - values[0] * v[2],
			values[0] * v[1] - values[1] * v[0]
		);
	}

	static vec3 cross(const vec3& a, const vec3& b) {
		return vec3(
			a[1] * b[2] - a[2] * b[1],
			a[2] * b[0] - a[0] * b[2],
			a[0] * b[1] - a[1] * b[0]
		);
	}

	float dot(const vec3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	float sqrLen() const {
		return x * x + y * y + z * z;
	}

	float len() const {
		return sqrtf(x * x + y * y + z * z);
	}

	float normalize() {
		const float l = sqrtf(x * x + y * y + z * z);
		if (l != 0) {
			x /= l;
			y /= l;
			z /= l;
		}

		return l;
	}

	vec3 normalized() const {
		const float l = sqrtf(x * x + y * y + z * z);
		return vec3(x / l, y / l, z / l);
	}

	static vec3 normalize(const vec3& v) {
		const float l = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
		return vec3(v.x / l, v.y / l, v.z / l);
	}

	static float dot(const vec3& a, const vec3& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	static float absCosTheta(const vec3 &a, const vec3 &b) {
	    return abs(dot(a, b));
	}

	vec3 orthogonal() const {
        return x == y ? vec3(-z, 0, x) : vec3(-y, x, 0);
    }

    static vec3 orthogonal(const vec3 &a) {
        vec3 res;
        if (fabsf(a.x) > fabsf(a.y)) {
            res = vec3(0, 1, 0);
        } else {
            res = vec3(1, 0, 0);
        }

        return cross(a, res).normalized();
    }

    // From Tungsten renderer
    static void gramSchmidt(vec3 &a, vec3 &b, vec3 &c) {
	    a.normalize();
        b -= a * a.dot(b);
        if (b.dot(b) < 1e-5) {
            b = orthogonal(a);
        } else {
            b.normalize();
        }

        c -= a * a.dot(c);
        c -= b * b.dot(c);
        if (dot(c, c) < 1e-5) {
            c = cross(a, b);
        } else {
            c.normalize();
        }
    }

	bool approxEquals(const vec3& rhs, const float precision = 1e-7) const {
		return fabsf(x - rhs.x) <= precision &&
			   fabsf(y - rhs.y) <= precision &&
			   fabsf(z - rhs.z) <= precision;
	}

	// https://en.cppreference.com/w/cpp/language/operators
	friend bool operator==(const vec3& lhs, const vec3& rhs) {
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	friend bool operator!=(const vec3& lhs, const vec3& rhs) {
		return !(lhs == rhs);
	}

	vec3& operator+=(const vec3& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;

		return *this;
	}

	friend vec3 operator+(vec3 lhs, const vec3& rhs) {
		lhs += rhs;
		return lhs;
	}

	vec3& operator+=(const float rhs) {
		x += rhs;
		y += rhs;
		z += rhs;

		return *this;
	}

	friend vec3 operator+(vec3 lhs, const float rhs) {
		lhs += rhs;
		return lhs;
	}

	vec3& operator-=(const vec3& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;

		return *this;
	}

	friend vec3 operator-(vec3 lhs, const vec3& rhs) {
		lhs -= rhs;
		return lhs;
	}

	vec3& operator-=(const float rhs) {
		x -= rhs;
		y -= rhs;
		z -= rhs;

		return *this;
	}

	friend vec3 operator-(vec3 lhs, const float rhs) {
		lhs -= rhs;
		return lhs;
	}

	vec3& operator*=(const vec3& rhs) {
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;

		return *this;
	}

	friend vec3 operator*(vec3 lhs, const vec3& rhs) {
		lhs *= rhs;
		return lhs;
	}

	vec3& operator*=(const float rhs) {
		x *= rhs;
		y *= rhs;
		z *= rhs;

		return *this;
	}

	friend vec3 operator*(vec3 lhs, const float rhs) {
		lhs *= rhs;
		return lhs;
	}

	vec3& operator/=(const vec3& rhs) {
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;

		return *this;
	}

	friend vec3 operator/(vec3 lhs, const vec3& rhs) {
		lhs /= rhs;
		return lhs;
	}

	vec3& operator/=(const float rhs) {
		x /= rhs;
		y /= rhs;
		z /= rhs;

		return *this;
	}

	friend vec3 operator/(vec3 lhs, const float rhs) {
		lhs /= rhs;
		return lhs;
	}

	vec3& operator=(const vec3& other) {
		// check for self-assignment
		if (&other == this) {
			return *this;
		}

		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	float operator [](int i) const {
		return values[i];
	}

	float& operator [](int i) {
		return values[i];
	}

	vec3 operator-() const {
		return vec3(-x, -y, -z);
	}
};

static inline vec3 fminf(const vec3& a, const vec3& b) {
	return vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

static inline vec3 fmaxf(const vec3& a, const vec3& b) {
	return vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

static inline float fminf(const vec3& a) {
	return fminf(fminf(a.x, a.y), a.z);
}

static inline float fmaxf(const vec3& a) {
	return fmaxf(fmaxf(a.x, a.y), a.z);
}

static std::ostream& operator<<(std::ostream& os, vec3 const& m) {
	return os << "{ " << m.x << ", " << m.y << ", " << m.z << " }";
}
