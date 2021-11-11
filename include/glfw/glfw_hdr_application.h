#pragma once
#include <glfw/glfw_application.h>
#include <opengl/texture.h>
#include <opengl/program.h>
#include <opengl/vao.h>
#include <opengl/vbo.h>

class GLFWHDRApplication : public GLFWApplication {
private:
    // HDR framebuffer handle
    GLuint hdrFBO = 0;
    GLTexture colorTex, depthTex;

    // Tone mapping
    GLProgram tonemapProgram = GLProgram("Tonemap Program");
    GLint screenTextureLoc = 0;
    GLint exposureLoc = 0;
    GLint gammaLoc = 0;

    // Full-screen quad geometry
    GLVao quadVao;
    GLVbo quadVbo;

    // Exposure and gamma
    float exposure = 1;
    float gamma = 2.2f;

protected:
	// Events
	virtual void onFramebufferSize(int width, int height);

	// Renders the HDR texture into LDR framebuffer
	void renderHDR();

public:
	GLFWHDRApplication();
	virtual ~GLFWHDRApplication();

	virtual void init(const std::map<int, int> &glfwWindowHints, const bool vsync = false) override;

	// Getters
    GLuint getHDRFramebuffer() const { return hdrFBO; }
    float getExposure() const { return exposure; }
    float getGamma() const { return gamma; }

    // Setters
    virtual void setExposure(const float value) { exposure = value; }
    virtual void setGamma(const float value) { gamma = value; }
};
