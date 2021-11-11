#pragma once
#include <utility>
#include <iostream>
#include <math/vec3.h>

// Row-major matrix
// Note: OpenGL uses column-major ordering
// Convention: matrices premultiply column vectors.
// Example: R * T * v (first translates then rotates)
class mat4 {
public:
	union {
		struct {
			float v00, v01, v02, v03;
			float v10, v11, v12, v13;
			float v20, v21, v22, v23;
			float v30, v31, v32, v33;
		};

		float v[16];
	};

	mat4(const mat4& a) {
		v[0] = a.v[0];
		v[1] = a.v[1];
		v[2] = a.v[2];
		v[3] = a.v[3];
		v[4] = a.v[4];
		v[5] = a.v[5];
		v[6] = a.v[6];
		v[7] = a.v[7];
		v[8] = a.v[8];
		v[9] = a.v[9];
		v[10] = a.v[10];
		v[11] = a.v[11];
		v[12] = a.v[12];
		v[13] = a.v[13];
		v[14] = a.v[14];
		v[15] = a.v[15];
	}

	mat4(mat4&&) = default;

	mat4() {
		 v[0] = 1;  v[1] = 0;  v[2] = 0;  v[3] = 0;
		 v[4] = 0;  v[5] = 1;  v[6] = 0;  v[7] = 0;
		 v[8] = 0;  v[9] = 0; v[10] = 1; v[11] = 0;
		v[12] = 0; v[13] = 0; v[14] = 0; v[15] = 1;
	}

	mat4(const float v0, const float v1, const float v2, const float v3,
		 const float v4, const float v5, const float v6, const float v7,
		 const float v8, const float v9, const float v10, const float v11,
		 const float v12, const float v13, const float v14, const float v15) {
		v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3;
		v[4] = v4; v[5] = v5; v[6] = v6; v[7] = v7;
		v[8] = v8; v[9] = v9; v[10] = v10; v[11] = v11;
		v[12] = v12; v[13] = v13; v[14] = v14; v[15] = v15;
	}

	vec3 right() const {
	    return vec3(v00, v10, v20);
	}

	vec3 up() const {
	    return vec3(v01, v11, v21);
	}

	vec3 forward() const {
        return vec3(v02, v12, v22);
	}

	vec3 translation() const {
	    return vec3(v03, v13, v23);
	}

	mat4 transposed() const {
		return mat4(
			v[0], v[4], v[8], v[12],
			v[1], v[5], v[9], v[13],
			v[2], v[6], v[10], v[14],
			v[3], v[7], v[11], v[15]
		);
	}

	void transpose() {
		std::swap(v[1], v[4]);
		std::swap(v[2], v[8]);
		std::swap(v[3], v[12]);
		std::swap(v[6], v[9]);
		std::swap(v[7], v[13]);
		std::swap(v[11], v[14]);
	}

	mat4 inverse() const {
	    mat4 temp;
	    inverted(temp);
	    return temp;
	}

    bool inverted(mat4 &inv) const {
        inv.v[0] =  v[5] * v[10] * v[15] - v[5] * v[14] * v[11] - v[6] * v[9] * v[15] + v[6] * v[13] * v[11] + v[7] * v[9] * v[14] - v[7] * v[13] * v[10];
        inv.v[1] = -v[1] * v[10] * v[15] + v[1] * v[14] * v[11] + v[2] * v[9] * v[15] - v[2] * v[13] * v[11] - v[3] * v[9] * v[14] + v[3] * v[13] * v[10];
        inv.v[2] =  v[1] * v[6] * v[15] - v[1] * v[14] * v[7] - v[2] * v[5] * v[15] + v[2] * v[13] * v[7] + v[3] * v[5] * v[14] - v[3] * v[13] * v[6];
        inv.v[3] = -v[1] * v[6] * v[11] + v[1] * v[10] * v[7] + v[2] * v[5] * v[11] - v[2] * v[9] * v[7] - v[3] * v[5] * v[10] + v[3] * v[9] * v[6];
        inv.v[4] = -v[4] * v[10] * v[15] + v[4] * v[14] * v[11] + v[6] * v[8] * v[15] - v[6] * v[12] * v[11] - v[7] * v[8] * v[14] + v[7] * v[12] * v[10];
        inv.v[5] =  v[0] * v[10] * v[15] - v[0] * v[14] * v[11] - v[2] * v[8] * v[15] + v[2] * v[12] * v[11] + v[3] * v[8] * v[14] - v[3] * v[12] * v[10];
        inv.v[6] = -v[0] * v[6] * v[15] + v[0] * v[14] * v[7] + v[2] * v[4] * v[15] - v[2] * v[12] * v[7] - v[3] * v[4] * v[14] + v[3] * v[12] * v[6];
        inv.v[7] =  v[0] * v[6] * v[11] - v[0] * v[10] * v[7] - v[2] * v[4] * v[11] + v[2] * v[8] * v[7] + v[3] * v[4] * v[10] - v[3] * v[8] * v[6];
        inv.v[8] =  v[4] * v[9] * v[15] - v[4] * v[13] * v[11] - v[5] * v[8] * v[15] + v[5] * v[12] * v[11] + v[7] * v[8] * v[13] - v[7] * v[12] * v[9];
        inv.v[9] = -v[0] * v[9] * v[15] + v[0] * v[13] * v[11] + v[1] * v[8] * v[15] - v[1] * v[12] * v[11] - v[3] * v[8] * v[13] + v[3] * v[12] * v[9];
        inv.v[10] =  v[0] * v[5] * v[15] - v[0] * v[13] * v[7] - v[1] * v[4] * v[15] + v[1] * v[12] * v[7] + v[3] * v[4] * v[13] - v[3] * v[12] * v[5];
        inv.v[11] = -v[0] * v[5] * v[11] + v[0] * v[9] * v[7] + v[1] * v[4] * v[11] - v[1] * v[8] * v[7] - v[3] * v[4] * v[9] + v[3] * v[8] * v[5];
        inv.v[12] = -v[4] * v[9] * v[14] + v[4] * v[13] * v[10] + v[5] * v[8] * v[14] - v[5] * v[12] * v[10] - v[6] * v[8] * v[13] + v[6] * v[12] * v[9];
        inv.v[13] =  v[0] * v[9] * v[14] - v[0] * v[13] * v[10] - v[1] * v[8] * v[14] + v[1] * v[12] * v[10] + v[2] * v[8] * v[13] - v[2] * v[12] * v[9];
        inv.v[14] = -v[0] * v[5] * v[14] + v[0] * v[13] * v[6] + v[1] * v[4] * v[14] - v[1] * v[12] * v[6] - v[2] * v[4] * v[13] + v[2] * v[12] * v[5];
        inv.v[15] =  v[0] * v[5] * v[10] - v[0] * v[9] * v[6] - v[1] * v[4] * v[10] + v[1] * v[8] * v[6] + v[2] * v[4] * v[9] - v[2] * v[8] * v[5];

        float det = v[0] * inv.v[0] + v[4] * inv.v[1] + v[8] * inv.v[2] + v[12] * inv.v[3];
        if (det == 0) {
            return false;
        }

        det = 1.f / det;
        for (int i = 0; i < 16; i++) {
            inv.v[i] *= det;
        }

        return true;
    }

    static mat4 rotateX(const float theta) {
		const float s = sinf(theta);
		const float c = cosf(theta);
		return mat4(
			1, 0, 0, 0,
			0, c, -s, 0,
			0, s, c, 0,
			0, 0, 0, 1
		);
	}

	static mat4 rotateY(const float theta) {
		const float s = sinf(theta);
		const float c = cosf(theta);
		return mat4(
			c, 0, s, 0,
			0, 1, 0, 0,
			-s, 0, c, 0,
			0, 0, 0, 1
		);
	}

	static mat4 rotateZ(const float theta) {
		const float s = sinf(theta);
		const float c = cosf(theta);
		return mat4(
			c, -s, 0, 0,
			s,  c, 0, 0,
			0,  0, 1, 0,
			0,  0, 0, 1
		);
	}

	// Euler XYZ rotation
    static mat4 rotate(const float rx, const float ry, const float rz) {
	    return rotateZ(rz) * rotateY(ry) * rotateX(rx);
	}

	// Axis-angle rotation
	// https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
	static mat4 rotateAxisAngle(const float angle, float x, float y, float z) {
        const float sinAngle = sinf(angle);
        const float cosAngle = cosf(angle);
        const float mag = sqrtf(x * x + y * y + z * z);

        if (mag > 0.0f) {
            x /= mag;
            y /= mag;
            z /= mag;

            const float xx = x * x;
            const float yy = y * y;
            const float zz = z * z;
            const float xy = x * y;
            const float yz = y * z;
            const float zx = z * x;
            const float xs = x * sinAngle;
            const float ys = y * sinAngle;
            const float zs = z * sinAngle;
            const float oneMinusCos = 1.0f - cosAngle;

            mat4 rotMat;
            rotMat.v00 = (oneMinusCos * xx) + cosAngle;
            rotMat.v01 = (oneMinusCos * xy) - zs;
            rotMat.v02 = (oneMinusCos * zx) + ys;
            rotMat.v03 = 0.0;

            rotMat.v10 = (oneMinusCos * xy) + zs;
            rotMat.v11 = (oneMinusCos * yy) + cosAngle;
            rotMat.v12 = (oneMinusCos * yz) - xs;
            rotMat.v13 = 0.0;

            rotMat.v20 = (oneMinusCos * zx) - ys;
            rotMat.v21 = (oneMinusCos * yz) + xs;
            rotMat.v22 = (oneMinusCos * zz) + cosAngle;
            rotMat.v23 = 0.0;

            rotMat.v30 = 0.0;
            rotMat.v31 = 0.0;
            rotMat.v32 = 0.0;
            rotMat.v33 = 1.0;

            return rotMat;
        }

        return mat4();
	}

	static mat4 scale(const float sx, const float sy, const float sz) {
		return mat4(
			sx, 0, 0, 0,
			0, sy, 0, 0,
			0, 0, sz, 0,
			0, 0,  0, 1
		);
	}

	static mat4 translate(const float tx, const float ty, const float tz) {
		return mat4(
			1, 0, 0, tx,
			0, 1, 0, ty,
			0, 0, 1, tz,
			0, 0, 0, 1
		);
	}

	// http://www.songho.ca/opengl/gl_camera.html#lookat
	// The camera is always located at the origin and facing the NEGATIVE z axis.
	// The up vector is recomputed to guarantee an ortho-normal coordinate system.
	static mat4 lookAt(const vec3& eye, const vec3& target, const vec3& up) {
		// Why (eye - target) instead of (target - eye)? Maybe because camera faces the negative z-axis
		const vec3 z = (eye - target).normalized();
		const vec3 x = up.cross(z).normalized();
		const vec3 y = z.cross(x);

		// This matrix transforms a point in world space into the space defined by (x, y, z) axes
		const mat4 mR = mat4(
			x.x, x.y, x.z, 0,
			y.x, y.y, y.z, 0,
			z.x, z.y, z.z, 0,
			0, 0, 0, 1
		);

		// This matrix translates a point by -eye
		const mat4 mT = translate(-eye.x, -eye.y, -eye.z);

		// First translate, then reorient (inverse ordering)
		return mR * mT;
	}

	// https://www.khronos.org/opengl/wiki/GluPerspective_code
	static mat4 perspective(float fovy, float aspectRatio, float znear, float zfar) {
		const float ymax = znear * tanf(fovy * 0.5f);
		const float xmax = ymax * aspectRatio;
		return frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
	}

	// http://www.songho.ca/opengl/gl_projectionmatrix.html#perspective
	static mat4 frustum(float left, float right, float bottom, float top, float znear, float zfar) {
		const float temp = 2 * znear;
		const float temp2 = right - left;
		const float temp3 = top - bottom;
		const float temp4 = zfar - znear;

		return mat4(
			temp / temp2, 0, (right + left) / temp2, 0,
			0, temp / temp3, (top + bottom) / temp3, 0,
			0, 0, (-zfar - znear) / temp4, (-temp * zfar) / temp4,
			0, 0, -1, 0
		);
	}

	// http://www.songho.ca/opengl/gl_projectionmatrix.html#ortho
	static mat4 ortho(float left, float right, float bottom, float top, float znear, float zfar) {
		return mat4(
			2 / (right-left), 0, 0, - (right + left) / (right - left),
			0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom),
			0, 0, -2 / (zfar-znear), -(zfar + znear) / (zfar - znear),
			0,0,0,1
		);
	}

	// Assumes matrix is orthogonal (i.e. M^-1 = M^t)
	// N' = N * (M^-1)^T = N * M^T^T = N * M
	static vec3 transform_direction(const mat4 &matrix, const vec3 &vector) {
		const float w = matrix.v[12] * vector.x + matrix.v[13] * vector.y + matrix.v[14] * vector.z + matrix.v[15] * 1;
		return vec3(
			(matrix.v[0] * vector.x + matrix.v[1] * vector.y + matrix.v[2] * vector.z) / w,
			(matrix.v[4] * vector.x + matrix.v[5] * vector.y + matrix.v[6] * vector.z) / w,
			(matrix.v[8] * vector.x + matrix.v[9] * vector.y + matrix.v[10] * vector.z) / w
		);
	}

	static vec3 transform_point(const mat4& matrix, const vec3& vector) {
		const float w = matrix.v[12] * vector.x + matrix.v[13] * vector.y + matrix.v[14] * vector.z + matrix.v[15] * 1;
		return vec3(
			(matrix.v[0] * vector.x + matrix.v[1] * vector.y + matrix.v[2] * vector.z + matrix.v[3] * 1) / w,
			(matrix.v[4] * vector.x + matrix.v[5] * vector.y + matrix.v[6] * vector.z + matrix.v[7] * 1) / w,
			(matrix.v[8] * vector.x + matrix.v[9] * vector.y + matrix.v[10] * vector.z + matrix.v[11] * 1) / w
		);
	}

	bool approxEquals(const mat4& rhs, const float precision = 1e-7) const {
	    for (int i = 0; i < 16; i++) {
	        if (fabsf(v[i] - rhs.v[i]) > precision) {
	            return false;
	        }
	    }

	    return true;
	}

	// https://en.cppreference.com/w/cpp/language/operators
	friend bool operator==(const mat4& lhs, const mat4& rhs) {
		return lhs.v[0] == rhs.v[0] &&
				lhs.v[1] == rhs.v[1] &&
				lhs.v[2] == rhs.v[2] &&
				lhs.v[3] == rhs.v[3] &&
				lhs.v[4] == rhs.v[4] &&
				lhs.v[5] == rhs.v[5] &&
				lhs.v[6] == rhs.v[6] &&
				lhs.v[7] == rhs.v[7] &&
				lhs.v[8] == rhs.v[8] &&
				lhs.v[9] == rhs.v[9] &&
				lhs.v[10] == rhs.v[10] &&
				lhs.v[11] == rhs.v[11] &&
				lhs.v[12] == rhs.v[12] &&
				lhs.v[13] == rhs.v[13] &&
				lhs.v[14] == rhs.v[14] &&
				lhs.v[15] == rhs.v[15];
	}

	friend bool operator!=(const mat4& lhs, const mat4& rhs) {
		return !(lhs == rhs);
	}

	mat4& operator*=(const mat4& rhs) {
		*this = (*this) * rhs;
		return *this;
	}

	// passing lhs by value helps optimize chained a*b*c
	friend mat4 operator*(mat4 lhs, const mat4& rhs) {
		return mat4(
			lhs.v[0] * rhs.v[0] + lhs.v[1] * rhs.v[4] + lhs.v[2] * rhs.v[8] + lhs.v[3] * rhs.v[12],
			lhs.v[0] * rhs.v[1] + lhs.v[1] * rhs.v[5] + lhs.v[2] * rhs.v[9] + lhs.v[3] * rhs.v[13],
			lhs.v[0] * rhs.v[2] + lhs.v[1] * rhs.v[6] + lhs.v[2] * rhs.v[10] + lhs.v[3] * rhs.v[14],
			lhs.v[0] * rhs.v[3] + lhs.v[1] * rhs.v[7] + lhs.v[2] * rhs.v[11] + lhs.v[3] * rhs.v[15],

			lhs.v[4] * rhs.v[0] + lhs.v[5] * rhs.v[4] + lhs.v[6] * rhs.v[8] + lhs.v[7] * rhs.v[12],
			lhs.v[4] * rhs.v[1] + lhs.v[5] * rhs.v[5] + lhs.v[6] * rhs.v[9] + lhs.v[7] * rhs.v[13],
			lhs.v[4] * rhs.v[2] + lhs.v[5] * rhs.v[6] + lhs.v[6] * rhs.v[10] + lhs.v[7] * rhs.v[14],
			lhs.v[4] * rhs.v[3] + lhs.v[5] * rhs.v[7] + lhs.v[6] * rhs.v[11] + lhs.v[7] * rhs.v[15],

			lhs.v[8] * rhs.v[0] + lhs.v[9] * rhs.v[4] + lhs.v[10] * rhs.v[8] + lhs.v[11] * rhs.v[12],
			lhs.v[8] * rhs.v[1] + lhs.v[9] * rhs.v[5] + lhs.v[10] * rhs.v[9] + lhs.v[11] * rhs.v[13],
			lhs.v[8] * rhs.v[2] + lhs.v[9] * rhs.v[6] + lhs.v[10] * rhs.v[10] + lhs.v[11] * rhs.v[14],
			lhs.v[8] * rhs.v[3] + lhs.v[9] * rhs.v[7] + lhs.v[10] * rhs.v[11] + lhs.v[11] * rhs.v[15],

			lhs.v[12] * rhs.v[0] + lhs.v[13] * rhs.v[4] + lhs.v[14] * rhs.v[8] + lhs.v[15] * rhs.v[12],
			lhs.v[12] * rhs.v[1] + lhs.v[13] * rhs.v[5] + lhs.v[14] * rhs.v[9] + lhs.v[15] * rhs.v[13],
			lhs.v[12] * rhs.v[2] + lhs.v[13] * rhs.v[6] + lhs.v[14] * rhs.v[10] + lhs.v[15] * rhs.v[14],
			lhs.v[12] * rhs.v[3] + lhs.v[13] * rhs.v[7] + lhs.v[14] * rhs.v[11] + lhs.v[15] * rhs.v[15]
		);
	}

	mat4& operator=(const mat4& other) {
		// check for self-assignment
		if (&other == this) {
			return *this;
		}

		v[0] = other.v[0];
		v[1] = other.v[1];
		v[2] = other.v[2];
		v[3] = other.v[3];
		v[4] = other.v[4];
		v[5] = other.v[5];
		v[6] = other.v[6];
		v[7] = other.v[7];
		v[8] = other.v[8];
		v[9] = other.v[9];
		v[10] = other.v[10];
		v[11] = other.v[11];
		v[12] = other.v[12];
		v[13] = other.v[13];
		v[14] = other.v[14];
		v[15] = other.v[15];
		return *this;
	}

	friend vec3 operator *(const mat4& a, const vec3& b) {
		const float w = a.v[12] * b.x + a.v[13] * b.y + a.v[14] * b.z + a.v[15] * 1;
		return vec3(
			(a.v[0] * b.x + a.v[1] * b.y + a.v[2] * b.z + a.v[3] * 1) / w,
			(a.v[4] * b.x + a.v[5] * b.y + a.v[6] * b.z + a.v[7] * 1) / w,
			(a.v[8] * b.x + a.v[9] * b.y + a.v[10] * b.z + a.v[11] * 1) / w
		);
	}
};

static std::ostream& operator<<(std::ostream& os, mat4 const& m) {
	return os << "{ " << m.v[0] << ", " << m.v[1] << ", " << m.v[2] << ", " << m.v[3] << " }" << std::endl <<
			     "{ " << m.v[4] << ", " << m.v[5] << ", " << m.v[6] << ", " << m.v[7] << " }" << std::endl <<
			     "{ " << m.v[8] << ", " << m.v[9] << ", " << m.v[10] << ", " << m.v[11] << " }" << std::endl <<
			     "{ " << m.v[12] << ", " << m.v[13] << ", " << m.v[14] << ", " << m.v[15] << " }" << std::endl;
}
