#version 400

in vec3 worldNormal;
in vec3 worldPosition;
in vec3 worldIrradiance;

// Eye position in world space
uniform vec3 eye;

// Mesh linear color
uniform vec3 meshColor;

// Light position in world space
uniform vec3 light;

// Light linear color
uniform vec3 lightColor;

// Light intensity
uniform float lightIntensity;

// Whether to compute per pixel lighting
uniform bool perPixel;

out vec4 fragColor;

void main(void) {
    if (perPixel) {
        vec3 L = light - worldPosition;
        float distSquared = dot(L, L);
        L = normalize(L);
        vec3 col = meshColor * lightColor * lightIntensity * max(0, dot(normalize(worldNormal), L)) / max(0.00001, distSquared);
        fragColor = vec4(col, 1);
    } else {
        fragColor = vec4(meshColor * worldIrradiance, 1);
    }
}
