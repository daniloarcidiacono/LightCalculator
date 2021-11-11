#include <glfw/glfw_application.h>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <chrono>

static void onGlfwError(int error, const char* description) {
    // https://www.glfw.org/docs/3.3/intro_guide.html#error_handling
    // Reported errors are never fatal. As long as GLFW was successfully initialized, it will remain initialized and in a safe state until terminated regardless of how many errors occur.
    fprintf(stderr, "[GLFW] -  %s\n", description);
}

// If we had omitted GLAPIENTRY in the function signature, the function would likely use the wrong calling convention, which causes all kinds of weird behaviour and crashes.
static void GLAPIENTRY onOpenGLError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    const char* severityStr;
    switch (severity) {
        // All OpenGL Errors, shader compilation / linking errors, or highly - dangerous undefined behavior
        case GL_DEBUG_SEVERITY_HIGH:
            severityStr = "HIGH";
            break;

        // Major performance warnings, shader compilation / linking warnings, or the use of deprecated functionality
        case GL_DEBUG_SEVERITY_MEDIUM:
            severityStr = "MEDIUM";
            break;

        // Redundant state change performance warning, or unimportant undefined behavior
        case GL_DEBUG_SEVERITY_LOW:
            severityStr = "LOW";
            break;

        // Anything that isn't an error or performance issue.
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            severityStr = "NOTIFICATION";
            break;

        default:
            severityStr = "UNKNOWN";
            break;
    }

    fprintf(stderr, "[%s] - %s\n", severityStr, message);
    //if (window != nullptr && severity == GL_DEBUG_SEVERITY_HIGH) {
    //    glfwSetWindowShouldClose(window, GL_TRUE);
    //}
}

GLFWApplication::GLFWApplication() {
}

GLFWApplication::~GLFWApplication() {
    if (window != nullptr) {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}

void GLFWApplication::update(const float delta) {
}

void GLFWApplication::onFPSUpdate() {
    // Display the frame count
    std::stringstream ss;
    ss << applicationName << "- [" << fps << " FPS @ " << viewportWidth << "x" << viewportHeight << "]";
    setTitle(ss.str());
}

void GLFWApplication::onMouseMove(double xpos, double ypos) {
}

void GLFWApplication::onMouseButton(int button, int action, int mods) {
}

void GLFWApplication::onMouseScroll(double xoffset, double yoffset) {
}

void GLFWApplication::onKeyboardEvent(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            close();
        }
    }
}

void GLFWApplication::onFramebufferSize(int width, int height) {
    viewportWidth = width;
    viewportHeight = height;
}

void GLFWApplication::init(const std::map<int, int> &glfwWindowHints, const bool vsync) {
    // Init GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Could not init GLFW");
    }

    bool isMaximized = false;
    glfwSetErrorCallback(onGlfwError);
    for (auto const& entry : glfwWindowHints) {
        if (entry.first == GLFW_MAXIMIZED && entry.second) {
            isMaximized = true;
        }

        glfwWindowHint(entry.first, entry.second);
    }

    window = glfwCreateWindow(viewportWidth, viewportHeight, applicationName.c_str(), NULL, NULL);
    if (!window) {
        // Window or OpenGL context creation failed
        throw std::runtime_error("Could not create window");
    }

    if (isMaximized) {
        glfwGetWindowSize(window, &viewportWidth, &viewportHeight);
    }

    // We need to call this before any OpenGL calls
    glfwMakeContextCurrent(window);
    glfwSwapInterval(vsync ? 1 : 0);
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    // Init GLEW
    GLenum result = glewInit();
    if (GLEW_OK != result) {
        std::string glewError = (const char *)glewGetErrorString(result);
        throw std::runtime_error(std::string("Could not init GLEW: ") + glewError);
    }

    // Setup debug
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        // Enabling GL_DEBUG_OUTPUT is probably superfluous, however, if we didn�t create a debug context, this would enable the debug output
        // (for as far as your OpenGL implementation likes to provide debug output in a non-debug context).
        glEnable(GL_DEBUG_OUTPUT);

        // Enabling GL_DEBUG_OUTPUT_SYNCHRONOUS is important. It guarantees that the callback is called by the same thread as the OpenGL api-call that invoked the callback.
        // Without it, the callback might be called from a different thread and thus you won�t be able to obtain a meaningful stack-trace in case of an error.
        // Enabling synchronous output might affect performance though.
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(onOpenGLError, nullptr);

        // Tells OpenGL that we want to receive all possible callback messages.
        //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_FALSE);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, NULL, GL_TRUE);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, NULL, GL_TRUE);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, NULL, GL_TRUE);
    }

    // FPS counter
    previousTime = glfwGetTime();
    frameDelta = 0;
    frameCount = 0;

    // https://stackoverflow.com/questions/39731561/use-lambda-as-glfwkeyfun
    glfwSetWindowUserPointer(window, this);

    // Setup callbacks
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
        GLFWApplication* self = (GLFWApplication*)glfwGetWindowUserPointer(window);
        self->onMouseMove(xpos, ypos);
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        GLFWApplication* self = (GLFWApplication*)glfwGetWindowUserPointer(window);
        self->onMouseButton(button, action, mods);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        GLFWApplication* self = (GLFWApplication*)glfwGetWindowUserPointer(window);
        self->onMouseScroll(xoffset, yoffset);
    });

    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        GLFWApplication *self = (GLFWApplication *)glfwGetWindowUserPointer(window);
        self->onKeyboardEvent(key, scancode, action, mods);
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
       GLFWApplication *self = (GLFWApplication *) glfwGetWindowUserPointer(window);

       // When the window is iconized, this callback is called with 0 for both width and height
       if (width > 0 || height > 0) {
           self->onFramebufferSize(width, height);
       }
   });
}

void GLFWApplication::run() {
    while (!glfwWindowShouldClose(window)) {
        const double currentTime = glfwGetTime();
        const double delta = currentTime - previousTime;

        glfwPollEvents();

        // Handle FPS
        frameCount++;
        if (delta >= 1) {
            fps = double(frameCount) / delta;

            onFPSUpdate();

            frameCount = 0;
            previousTime = glfwGetTime();
        }

        // Update
        update(frameDelta);

        // Swap the buffers
        glfwSwapBuffers(window);
        frameDelta = glfwGetTime() - currentTime;
    }
}

void GLFWApplication::close() {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void GLFWApplication::setTitle(const std::string& title) {
    glfwSetWindowTitle(window, title.c_str());
}
