#include <math/uber_camera.h>
#include <math/spherical.h>

UberCamera::UberCamera() {
}

static void sphericalClamp(vec3 &spherical) {
    const float EPS = 1e-4;
    spherical.y = clamp(spherical.y, 0, M_PI, EPS);
}

void UberCamera::computeBasis(vec3 &front, vec3 &up, vec3 &right) const {
    front = (target - eye).normalized();
    up = -Spherical::dThetaCartesian(Spherical::toSpherical(front)).normalized();
    right = vec3::cross(front, up);
}

void UberCamera::computeMatrices() {
    vec3 front, up, right;
    computeBasis(front, up, right);

    // View matrix
    viewMatrix = mat4::lookAt(eye, target, up);

    // Proj matrix
    projMatrix = mat4::perspective(fovY, aspect, zNear, zFar);

    // ViewProj Matrix
    viewProjMatrix = projMatrix * viewMatrix;

    // Reset dirty flag
    needsUpdate = false;
}

bool UberCamera::isDirty() const {
    return needsUpdate;
}

void UberCamera::pan(const float x, const float y, const float z) {
    vec3 front, up, right;
    computeBasis(front, up, right);

    const vec3 delta = x * right + y * up + z * front;
    target += delta;
    eye += delta;
    needsUpdate = true;
}

void UberCamera::rotate(const float x, const float y) {
    if (type == ARCBALL) {
        vec3 sp = Spherical::toSpherical(eye - target);
        sp.y += x;
        sp.z += y;
        sphericalClamp(sp);
        sp = Spherical::toCartesian(sp);
        eye = target + sp;
    } else {
        vec3 sp = Spherical::toSpherical(target - eye);
        sp.y -= x;
        sp.z += y;
        sphericalClamp(sp);
        sp = Spherical::toCartesian(sp);
        target = eye + sp;
    }

    needsUpdate = true;
}

void UberCamera::zoom(const float r) {
    float distance = (target - eye).len();
    distance *= r;

    vec3 front, up, right;
    computeBasis(front, up, right);
    eye = target - front * distance;

    needsUpdate = true;
}

const mat4 &UberCamera::getViewMatrix() {
    if (needsUpdate) {
        computeMatrices();
    }

    return viewMatrix;
}

const mat4 &UberCamera::getProjMatrix() {
    if (needsUpdate) {
        computeMatrices();
    }

    return projMatrix;
}

const mat4 &UberCamera::getViewProjMatrix() {
    if (needsUpdate) {
        computeMatrices();
    }

    return viewProjMatrix;
}

void UberCamera::setType(const Type type) {
    if (type != this->type) {
        this->type = type;
        needsUpdate = true;
    }
}

void UberCamera::setEye(const vec3 &eye) {
    if (eye != this->eye) {
        this->eye = eye;
        needsUpdate = true;
    }
}

void UberCamera::setTarget(const vec3 &target) {
    if (target != this->target) {
        this->target = target;
        needsUpdate = true;
    }
}

void UberCamera::setFovY(const float fovY) {
    if (fovY != this->fovY) {
        this->fovY = fovY;
        needsUpdate = true;
    }
}

void UberCamera::setZNear(const float zNear) {
    if (zNear != this->zNear) {
        this->zNear = zNear;
        needsUpdate = true;
    }
}

void UberCamera::setZFar(const float zFar) {
    if (zFar != this->zFar) {
        this->zFar = zFar;
        needsUpdate = true;
    }
}

void UberCamera::setAspect(const float aspect) {
    if (aspect != this->aspect) {
        this->aspect = aspect;
        needsUpdate = true;
    }
}

