#pragma once
#include <math/vec3.h>
#include <math/mat4.h>
#include <math/includes.h>

/**
 * Camera that acts both as first-person or arcball.
 */
class UberCamera {
public:
    enum Type {
        FPS, ARCBALL
    };

private:
    Type type = ARCBALL;

    // Eye in world space
    vec3 eye = vec3(0, 0, 1);

    // Target in world space
    vec3 target = vec3(0, 0, 0);

    // Vertical field of view in radians
    float fovY = 60.0_deg;
    // Note: zNear and zFar values influence the precision of the depth buffer (https://www.khronos.org/opengl/wiki/Depth_Buffer_Precision)
    float zNear = 0.01;
    float zFar = 100;
    float aspect = 1;

    // Dirty flag
    bool needsUpdate = true;

    // Cached data
    mat4 viewMatrix, projMatrix, viewProjMatrix;

    void computeBasis(vec3 &front, vec3 &up, vec3 &right) const;
    void computeMatrices();

public:
    UberCamera();

    bool isDirty() const;

    void pan(const float x, const float y, const float z);
    void rotate(const float x, const float y);
    void zoom(const float r);

    // Getters
    Type getType() const { return type; }
    const vec3 &getEye() const { return eye; }
    const vec3 &getTarget() const { return target; }
    float getFovY() const { return fovY; }
    float getZNear() const { return zNear; }
    float getZFar() const { return zFar; }
    float getAspect() const { return aspect; }
    const mat4 &getViewMatrix();
    const mat4 &getProjMatrix();
    const mat4 &getViewProjMatrix();

    // Setters
    void setType(const Type type);
    void setEye(const vec3 &eye);
    void setTarget(const vec3 &target);
    void setFovY(const float fovY);
    void setZNear(const float zNear);
    void setZFar(const float zFar);
    void setAspect(const float aspect);
};
