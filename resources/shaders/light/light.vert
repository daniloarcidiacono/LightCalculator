#version 400

layout(location = 0) in vec3 vertices;
layout(location = 1) in vec3 vertexNormal;
uniform mat4 modelViewProjection;
uniform mat4 model;

out vec3 worldPosition;
out vec3 worldNormal;

void main(void) {
    // The inverse and the transpose cancel out if the model matrix consist only of translation,
    // rotation and uniform scaling, leaving only mat3(model).
    mat3 normalMatrix = transpose(inverse(mat3(model)));

    // Compute world-space position and normal
    worldPosition = vec3(model * vec4(vertices, 1));
    worldNormal = normalize(normalMatrix * vertexNormal);

    // Output projected vertex
    gl_Position = modelViewProjection * vec4(vertices, 1);
}
