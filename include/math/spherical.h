#pragma once
#include <math/includes.h>
#include <math/vec3.h>

/**
 * Utilities for spherical coordinate system (physics notation).
 * spherical = (radius, inclination, azimuth)
 * with r = [0; +inf), inclination (theta) = [0; pi], azimuth (phi) = [0, 2pi)
 * y axis points upwards
 */
class Spherical {
private:
    Spherical() = default;

public:
    // Converts a spherical coordinate to a cartesian coordinate.
    static vec3 toCartesian(const vec3& spherical) {
        const float sTheta = sin(spherical.y);
        const float sPhi = sin(spherical.z);
        const float cTheta = cos(spherical.y);
        const float cPhi = cos(spherical.z);

        return vec3(
            spherical.x * sTheta * sPhi,
            spherical.x * cTheta,
            spherical.x * sTheta * cPhi
        );
    }

    static vec3 dThetaCartesian(const vec3& spherical) {
        const float sTheta = sin(spherical.y);
        const float sPhi = sin(spherical.z);
        const float cTheta = cos(spherical.y);
        const float cPhi = cos(spherical.z);

        return vec3(
            spherical.x * cTheta * sPhi,
            -spherical.x * sTheta,
            spherical.x * cTheta * cPhi
        );
    }

    // Converts a cartesian coordinate to a spherical coordinate (center is assumed to be at origin).
    static vec3 toSpherical(const vec3& cartesian) {
        const float r = cartesian.len();
        vec3 result = vec3(
            r,
            acos(cartesian.y / r),
            atan2(cartesian.x, cartesian.z)
        );

        // Math.atan2 codomain is (-pi; pi), not (0; 2pi)
        if (result.z < 0) {
            result.z += 2 * M_PI;
        }

        return result;
    }
};
