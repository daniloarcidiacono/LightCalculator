#include <glfw/glfw_hdr_application.h>
#include <iostream>

static const std::string TONEMAP_VERT = R"(
#version 400
layout(location = 0) in vec2 vertices;
layout(location = 1) in vec2 vertexUV;

out vec2 TexCoords;

void main() {
    gl_Position = vec4(vertices.x, vertices.y, 0.0, 1.0);
    TexCoords = vertexUV;
}
)";

static const std::string TONEMAP_FRAG = R"(
#version 400

in vec2 TexCoords;
uniform sampler2D screenTexture;
uniform float gamma;
uniform float exposure;

out vec4 fragColor;

void main(void) {
    // Read fragment from HDR texture
    vec3 fragment = texture(screenTexture, TexCoords).rgb;

    // Tonemap
//    fragment = vec3(1.0f) - exp(-fragment * exposure);

    // Gamma correct
    fragment = pow(fragment, vec3(1.0f / gamma));

    // Output
    fragColor = vec4(fragment, 1.0f);
}
)";

struct vec2 {
    float x, y;
    vec2(float x, float y)
        : x(x), y(y) {
    }
};

struct QuadVertex {
    vec2 vertex;
    vec2 uv;

    QuadVertex(const vec2 &vertex, const vec2 &uv)
        : vertex(vertex), uv(uv) {
    }
};

GLFWHDRApplication::GLFWHDRApplication()
    : GLFWApplication() {
}

GLFWHDRApplication::~GLFWHDRApplication() {
    if (hdrFBO != 0) {
        glDeleteFramebuffers(1, &hdrFBO);
    }
}

void GLFWHDRApplication::onFramebufferSize(int width, int height) {
    GLFWApplication::onFramebufferSize(width, height);
    colorTex.resize(width, height);
    depthTex.resize(width, height);
}

void GLFWHDRApplication::renderHDR() {
    tonemapProgram.use();
    glUniform1i(screenTextureLoc, 0);
    glUniform1f(exposureLoc, exposure);
    glUniform1f(gammaLoc, gamma);
    colorTex.bind(GL_TEXTURE0);
    quadVao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
//    quadVao.unbind();
//    colorTex.unbind();
//    glUseProgram(0);
}

void GLFWHDRApplication::init(const std::map<int, int> &glfwWindowHints, const bool vsync) {
    GLFWApplication::init(glfwWindowHints, vsync);

    // TODO: How about these?
//    glEnable(GL_MULTISAMPLE);
//    glEnable(GL_FRAMEBUFFER_SRGB);

    // Create the HDR framebuffer
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    // Create color buffer
    colorTex.create(getViewportWidth(), getViewportHeight(), GLTextureFormat::RGBA32F)
        .bind(GL_TEXTURE0)
        .setTextureFilters(GL_LINEAR, GL_LINEAR)
        .setTextureWraps(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    // Create depth buffer
    // TODO: Use renderbuffers? They can't be read by shaders, but they are faster. Can they be resized though?
    //  glGenRenderBuffers(1, &RBO),
    //  glBindRenderBuffer(GL_RENDERBUFFER, RBO),
    //  glRenderBuferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height)
    //  glFramebufferRenderBuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER; RBO);
    depthTex.create(getViewportWidth(), getViewportHeight(), GLTextureFormat::DEPTHF)
        .bind(GL_TEXTURE0)
        .setTextureFilters(GL_LINEAR, GL_LINEAR)
        .setTextureWraps(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    // Attach
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex.getTextureId(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex.getTextureId(), 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error(std::string("Could not create HDR framebuffer"));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Compile and link tonemap program
    tonemapProgram.compileShader(TONEMAP_FRAG, GLShaderType::FRAGMENT);
    tonemapProgram.compileShader(TONEMAP_VERT, GLShaderType::VERTEX);
    tonemapProgram.link();
    screenTextureLoc = glGetUniformLocation(tonemapProgram.getProgramId(), "screenTexture");
    exposureLoc = glGetUniformLocation(tonemapProgram.getProgramId(), "exposure");
    gammaLoc = glGetUniformLocation(tonemapProgram.getProgramId(), "gamma");

    // Init quad geometry
    // Without transforms, the OpenGL camera is facing the negative Z axis.
    // OpenGL uses a right-handed coordinate system, and the UV (0, 0) refers to the bottom left of the image:
    //
    //             Y ^
    //               |
    //         0,1   |   1,1
    //          A ------- D
    //          |    |    |
    //          |    |----|----> X
    //          |  Image  |
    //          |         |
    //          B ------- C
    //         0,0       1,0
    //
    // Triangles are in CCW order: (A, B, C), (A, C, D)
    quadVbo.create<QuadVertex>(GL_STATIC_DRAW, {
        // A, B, C
        QuadVertex(vec2(-1,  1), vec2(0, 1)),
        QuadVertex(vec2(-1, -1),  vec2(0, 0)),
        QuadVertex(vec2(1, -1),  vec2(1, 0)),

        // A, C, D
        QuadVertex(vec2(-1,  1),  vec2(0, 1)),
        QuadVertex(vec2(1, -1),  vec2(1, 0)),
        QuadVertex(vec2(1,  1),  vec2(1, 1))
    }).bind();
    quadVao.create().bind();
    quadVao.vertexAttribPointer(0, GLVertexAttributeFormat::POS2F, 0, quadVbo);
    quadVao.vertexAttribPointer(1, GLVertexAttributeFormat::TEXCOORD2F, 2 * sizeof(float), quadVbo);
    quadVbo.unbind();
    quadVao.unbind();
}
