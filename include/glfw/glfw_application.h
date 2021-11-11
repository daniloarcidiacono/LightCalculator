#pragma once
#include <opengl/includes.h>
#include <map>
#include <string>

class GLFWApplication {
private:
	int viewportWidth = 512;
	int viewportHeight = 512;
	std::string applicationName;
	GLFWwindow* window = nullptr;
	double previousTime = 0, frameDelta = 0;
	unsigned int frameCount = 0;
	float fps = 0;

protected:
	// Main update function
	virtual void update(const float delta);

	// Events
	virtual void onFPSUpdate();
    virtual void onMouseMove(double xpos, double ypos);
	virtual void onMouseButton(int button, int action, int mods);
	virtual void onMouseScroll(double xoffset, double yoffset);
	virtual void onKeyboardEvent(int key, int scancode, int action, int mods);
	virtual void onFramebufferSize(int width, int height);

	// Sets the window title
	void setTitle(const std::string& title);

	// Closes the window
	void close();

public:
	GLFWApplication();
	virtual ~GLFWApplication();

	virtual void init(const std::map<int, int> &glfwWindowHints, const bool vsync = false);
	virtual void run();

	// Setters
	void setApplicationName(const std::string &applicationName) { this->applicationName = applicationName; }

	// Getters
	int getViewportWidth() const { return viewportWidth; }
	int getViewportHeight() const { return viewportHeight; }
	const std::string& getApplicationName() const { return applicationName; }
	float getFPS() const { return fps; }
    GLFWwindow *getWindow() const { return window; }
};
