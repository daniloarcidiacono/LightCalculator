#version 400

in vec3 worldNormal;
in vec3 worldPosition;
uniform vec3 eye;
uniform vec3 color;

out vec4 fragColor;

void main(void) {
    vec3 L = normalize(eye - worldPosition);
    float lambert = max(abs(dot(normalize(worldNormal), L)), 0);
    fragColor = vec4(color * lambert, 1);
}
