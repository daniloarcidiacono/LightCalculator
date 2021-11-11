#define CHECK_LEAKS
#ifdef CHECK_LEAKS
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#endif
#include <glfw/glfw_hdr_application.h>
#include <math/uber_camera.h>
#include <math/triangle.h>
#include <math/light.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>
#include <imgui/includes.h>
#include <cmrc/cmrc.hpp>
#include <iostream>
#include <random>

// https://stackoverflow.com/questions/30024364/any-ways-to-run-program-debugging-in-visual-studio-on-nvidia-graphics-card
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

static const std::map<int, int> windowHints = {
#ifdef NDEBUG
	{ GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE },
#else
	{ GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE },
#endif
	{ GLFW_CONTEXT_VERSION_MAJOR, 4 },
	{ GLFW_CONTEXT_VERSION_MINOR, 3 },
	{ GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE },
	{ GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE },
	{ GLFW_RESIZABLE, GL_TRUE },
	{ GLFW_MAXIMIZED, GL_TRUE },
	{ GLFW_DEPTH_BITS, 32 },
	{ GLFW_STENCIL_BITS, 0 }
	// { GLFW_SRGB_CAPABLE, GL_TRUE },
};

// CMake RC filesystem
CMRC_DECLARE(LightCalculator);

static cmrc::embedded_filesystem fs = cmrc::LightCalculator::get_filesystem();
static std::string load(const std::string &path) {
    cmrc::file resource = fs.open(path);
    return std::string(resource.begin(), resource.end());
}

// Random numbers
static std::random_device rd;
static std::mt19937 mt(rd());
static std::uniform_real_distribution<float> uniform(0.0f, 1.0f);

// Computes Integral[light.color * light.intensity * max(0, N . L) / distance^2, dA] / TriangleAreas using MonteCarlo
static vec3 computeIrradiance(const std::vector<Triangle> &triangles, const Light &light, const int samples) {
    std::uniform_int_distribution<int> triangleDist(0, triangles.size() - 1);

    vec3 integral = vec3(0.);
    const float trianglePickProbability = 1.0f / triangles.size();
    for (int i = 0; i < samples; i++) {
        // Pick a triangle
        const int triangleIndex = triangleDist(mt);
        const Triangle &triangle = triangles.at(triangleIndex);

        // Generate a random sample
        const vec3 point = triangle.uniformSample(uniform(mt), uniform(mt));
        const vec3 N = triangle.normal();
        const vec3 L = vec3::normalize(light.position - point);
        const float distanceSquared = (light.position - point).sqrLen();
        const float nDotL = vec3::dot(N, L);
        const float sampleProbability = trianglePickProbability * triangle.uniformSamplePdf();
        integral += light.color * light.intensity * (std::max(0.0f, nDotL) / std::max(0.00001f, distanceSquared)) / sampleProbability;
    }

    // Result of MC integration (Integral[N . L, dA])
    integral /= (float)samples;

    // We also divide by the total area (not related to MC)
    float totalArea = 0;
    for (const Triangle &triangle : triangles) {
        totalArea += triangle.area();
    }

    return integral / totalArea;
}

// Converts a color to linear space
static vec3 toLinear(const vec3 &v, const float gamma) {
    return vec3(powf(v.x, gamma), powf(v.y, gamma), powf(v.z, gamma));
}

// Converts a color to gamma space
static vec3 toGamma(const vec3 &v, const float gamma) {
    return vec3(powf(v.x, 1.0f / gamma), powf(v.y, 1.0f / gamma), powf(v.z, 1.0f / gamma));
}

namespace ImGui {
    // Helper to display a little (?) mark which shows a tooltip when hovered.
    // In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
    static void HelpMarker(const char *desc) {
        TextDisabled("(?)");
        if (IsItemHovered()) {
            BeginTooltip();
            PushTextWrapPos(GetFontSize() * 35.0f);
            TextUnformatted(desc);
            PopTextWrapPos();
            EndTooltip();
        }
    }
}

class HdrApp : public GLFWHDRApplication {
private:
    struct MeshVertex {
        vec3 position;
        vec3 normal;
        vec3 irradiance;

        MeshVertex() {
        }

        MeshVertex(const vec3 &position, const vec3 &normal, const vec3 &irradiance)
            : position(position), normal(normal), irradiance(irradiance) {
        }
    };

    struct LightVertex {
        vec3 position;
        vec3 normal;

        LightVertex() {
        }

        LightVertex(const vec3 &position, const vec3 &normal)
                : position(position), normal(normal) {
        }
    };

private:
    // Camera
    UberCamera camera;

    // Model paths
    std::vector<std::string> models;

    // Light shader
    GLProgram lightProgram = GLProgram("Light Program");
    GLuint lightModelLoc = 0;
    GLuint lightModelViewProjLoc = 0;
    GLuint lightEyeLoc = 0;
    GLuint lightColorLoc = 0;

    // Light geometry
    GLVao lightVao;
    GLVbo lightVbo;
    int lightTriangles = 0;

    // Mesh shader
    GLProgram meshProgram = GLProgram("Mesh Program");
    GLuint meshModelLoc = 0;
    GLuint meshModelViewProjLoc = 0;
    GLuint meshEyeLoc = 0;
    GLuint meshMeshColorLoc = 0;
    GLuint meshLightLoc = 0;
    GLuint meshLightColorLoc = 0;
    GLuint meshLightIntensityLoc = 0;
    GLuint meshPerPixelLoc = 0;

    // Mesh geometry
    GLVao meshVao;
    GLVbo meshVbo;
    int meshTriangles = 0;
    std::vector<MeshVertex> vertices;

    // Interaction
    bool middleButtonPressed = false;
    bool shiftPressed = false;
    float lastMouseX = 0, lastMouseY = 0;
    bool showUi = true;
    bool renderModelWireframe = false;
    bool perPixelLighting = true;
    int modelIndex = -1;

    // Render settings
    int samples = 1000;
    Light light = Light(vec3(0, 1, 0), vec3(1, 1, 1), 1);
    vec3 meshColor = vec3(1, 1, 1);

protected:
    void updateViewport(int width, int height) {
        glViewport(0, 0, width, height);

        const float aspect = width / float(height);
        camera.setAspect(aspect);
    }

    void update(const float delta) override final {
        // Render in HDR
        glBindFramebuffer(GL_FRAMEBUFFER, getHDRFramebuffer());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderScene();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Tonemapping
        glClear(GL_DEPTH_BUFFER_BIT);
        renderHDR();

        // LDR Overlays
        if (showUi) {
            renderUI();
        }
    }

    void onFramebufferSize(int width, int height) override final {
        std::cout << "Resizing (width = " << width << ", height = " << height << ")" << std::endl;
        updateViewport(width, height);

        GLFWHDRApplication::onFramebufferSize(width, height);
    }

    void onMouseButton(int button, int action, int mods) override final {
        if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
            middleButtonPressed = action == GLFW_PRESS;
        }
    }

    void onMouseMove(double xpos, double ypos) override final {
        const float deltaX = xpos - lastMouseX;
        const float deltaY = ypos - lastMouseY;
        if (middleButtonPressed) {
            if (!shiftPressed) {
                camera.rotate(deltaY * -0.005, deltaX * -0.005);
            } else {
                camera.pan(deltaX * -0.005, deltaY * 0.005, 0);
            }
        }

        lastMouseX = xpos;
        lastMouseY = ypos;
    }

    void onMouseScroll(double xoffset, double yoffset) override {
        ImGuiIO &io = ImGui::GetIO();
        if (showUi && io.WantCaptureMouse) {
            return;
        }

        if (camera.getType() == UberCamera::ARCBALL) {
            camera.zoom(1 - yoffset * 0.2);
        } else {
            camera.pan(0, 0, yoffset * 0.1);
        }
    }

    void onKeyboardEvent(int key, int scancode, int action, int mods) override {
        ImGuiIO &io = ImGui::GetIO();
        if (showUi && io.WantCaptureKeyboard) {
            return;
        }

        if (key == GLFW_KEY_LEFT_SHIFT) {
            shiftPressed = action != GLFW_RELEASE;
        }

        if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
            showUi = !showUi;
        }

        if (key == GLFW_KEY_W && action == GLFW_PRESS) {
            renderModelWireframe = !renderModelWireframe;
        }

        if (key == GLFW_KEY_C && action == GLFW_PRESS) {
            if (camera.getType() == UberCamera::FPS) {
                camera.setType(UberCamera::ARCBALL);
            } else {
                camera.setType(UberCamera::FPS);
            }
        }
    }

    void cleanMesh() {
        // Clean
        meshVbo.destroy();
        meshVao.destroy();
        vertices.clear();
    }

    void loadMesh(const std::string &fileName) {
        cleanMesh();

        // Load OBJ
        const std::string model = load(fileName);
        std::string err;
        std::istringstream is(model);
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        tinyobj::LoadObj(&attrib, &shapes, nullptr, &err, &is, nullptr, false);
        if (!err.empty()) {
            throw std::runtime_error("Failed to load " + fileName + ": " + err);
        }

        if (shapes.empty()) {
            throw std::runtime_error("Model has no meshes!");
        }

        // Transform shape into OpenGL objects
        const tinyobj::shape_t &mainShape = shapes.at(0);
        std::cout << "Importing shape " << mainShape.name << std::endl;
        for (const tinyobj::index_t &index : mainShape.mesh.indices) {
            vertices.emplace_back(
                MeshVertex(
                    vec3(
                        attrib.vertices[3 * index.vertex_index],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    ),
                    vec3(
                        attrib.normals[3 * index.normal_index],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    ),
                    vec3()
                )
            );
        }

        meshTriangles = vertices.size() / 3;
        meshVbo
            .create<MeshVertex>(GL_STATIC_DRAW, vertices.size(), vertices.data())
            .bind();
        meshVao
            .create()
            .bind()
            .vertexAttribPointer(0, GLVertexAttributeFormat::POS3F, 0, meshVbo)
            .vertexAttribPointer(1, GLVertexAttributeFormat::NORMAL3F, 3 * sizeof(float), meshVbo)
            .vertexAttribPointer(2, GLVertexAttributeFormat::COL3F, 6 * sizeof(float), meshVbo)
            .unbind();
        meshVbo.unbind();
    }

    void computeLighting() {
        std::cout << "Computing irradiances using " << samples << " samples" << std::endl;
        for (int i = 0; i < meshTriangles; i++) {
            MeshVertex &v0 = vertices[3 * i];
            MeshVertex &v1 = vertices[3 * i + 1];
            MeshVertex &v2 = vertices[3 * i + 2];

            std::vector<Triangle> triangles;
            triangles.emplace_back(Triangle(v0.position, v1.position, v2.position));

            const vec3 irradiance = computeIrradiance(triangles, light, samples);
            v0.irradiance = irradiance;
            v1.irradiance = irradiance;
            v2.irradiance = irradiance;
            setTitle("Computing lighting (" + std::to_string(i + 1) + " of " + std::to_string(meshTriangles) + ")");
        }

        meshVbo.bind();
        meshVbo.update(vertices.data(), vertices.size());
        meshVbo.unbind();
    }

    void loadLightGizmo(const std::string &fileName) {
        const std::string model = load(fileName);

        std::string err;
        std::istringstream is(model);
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        tinyobj::LoadObj(&attrib, &shapes, nullptr, &err, &is, nullptr, false);
        if (!err.empty()) {
            throw std::runtime_error("Failed to load " + fileName + ": " + err);
        }

        if (shapes.empty()) {
            throw std::runtime_error("Model has no meshes!");
        }

        const tinyobj::shape_t &mainShape = shapes.at(0);
        std::vector<LightVertex> lightVertices;
        std::cout << "Importing shape " << mainShape.name << std::endl;
        for (const tinyobj::index_t &index : mainShape.mesh.indices) {
            lightVertices.emplace_back(
                LightVertex(
                    vec3(
                        attrib.vertices[3 * index.vertex_index],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    ),
                    vec3(
                        attrib.normals[3 * index.normal_index],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    )
                )
            );
        }
        lightTriangles = lightVertices.size() / 3;

        lightVbo
            .create<LightVertex>(GL_STATIC_DRAW, lightVertices.size(), lightVertices.data())
            .bind();
        lightVao
            .create()
            .bind()
            .vertexAttribPointer(0, GLVertexAttributeFormat::POS3F, 0, lightVbo)
            .vertexAttribPointer(1, GLVertexAttributeFormat::NORMAL3F, 3 * sizeof(float), lightVbo)
            .unbind();
        lightVbo.unbind();
    }

    void setupScene() {
        camera.setEye(vec3(0, 0, -5));

        const std::string lightFileName = R"(resources/models/light.obj)";
        loadLightGizmo(lightFileName);

        changeCurrentModel(2);
    }

    void renderScene() {
        // Render mesh
        if (modelIndex != -1) {
            meshProgram.use();
            mat4 meshModelMatrix;
            glUniformMatrix4fv(meshModelLoc, 1, GL_TRUE, (GLfloat *)(&meshModelMatrix.v[0]));
            glUniformMatrix4fv(meshModelViewProjLoc, 1, GL_TRUE, (GLfloat *)(&camera.getViewProjMatrix().v[0]));
            glUniform3fv(meshEyeLoc, 1, camera.getEye().values);
            glUniform3fv(meshMeshColorLoc, 1, meshColor.values);
            glUniform3fv(meshLightLoc, 1, light.position.values);
            glUniform3fv(meshLightColorLoc, 1, light.color.values);
            glUniform1f(meshLightIntensityLoc, light.intensity);
            glUniform1i(meshPerPixelLoc, perPixelLighting);

            meshVao.bind();
            if (renderModelWireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }

            glDrawArrays(GL_TRIANGLES, 0, 3 * meshTriangles);

            if (renderModelWireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        // Render light
        lightProgram.use();
        const mat4 lightModelMatrix = mat4::translate(light.position.x, light.position.y, light.position.z) * mat4::scale(0.1, 0.1, 0.1);
        const mat4 lightMVPMatrix = camera.getViewProjMatrix() * lightModelMatrix;
        glUniformMatrix4fv(lightModelLoc, 1, GL_TRUE, (GLfloat *)(&lightModelMatrix.v[0]));
        glUniformMatrix4fv(lightModelViewProjLoc, 1, GL_TRUE, (GLfloat *)(&lightMVPMatrix.v[0]));
        glUniform3fv(lightEyeLoc, 1, camera.getEye().values);
        glUniform3fv(lightColorLoc, 1, light.color.values);
        lightVao.bind();
        if (renderModelWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        glDrawArrays(GL_TRIANGLES, 0, 3 * lightTriangles);

        if (renderModelWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    void renderUI() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // If the window is not collapsed...
        if (ImGui::Begin("Settings", &showUi)) {
            ImGui::Text("Viewport: %dx%d", getViewportWidth(), getViewportHeight());
            ImGui::Text("FPS: %.2f", getFPS());
            ImGui::Separator();
            ImGui::BulletText("Mouse wheel: zoom");
            ImGui::BulletText("Drag mouse wheel: rotate");
            ImGui::BulletText("Shift + drag mouse wheel: pan");
            ImGui::BulletText("W: Toggle wireframe");
            ImGui::BulletText("C: Toggle arcball/first person");
            ImGui::BulletText("TAB: Toggle this window");
            ImGui::Spacing();

            if (ImGui::CollapsingHeader("Rendering")) {
                if (ImGui::BeginCombo("Model", modelIndex != -1 ? models.at(modelIndex).c_str() : "<none>")) {
                    if (ImGui::Selectable("<none>", modelIndex == -1)) {
                        changeCurrentModel(-1);
                    }

                    if (modelIndex == -1) {
                        ImGui::SetItemDefaultFocus();
                    }

                    for (int i = 0; i < models.size(); i++) {
                        const bool isSelected = modelIndex == i;
                        const std::string &model = models.at(i);
                        if (ImGui::Selectable(model.c_str(), isSelected)) {
                            changeCurrentModel(i);
                        }

                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }

                    ImGui::EndCombo();
                }

                vec3 localMeshColor = toGamma(meshColor, getGamma());
                if (ImGui::ColorEdit3("Mesh color", localMeshColor.values, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoAlpha)) {
                    meshColor = toLinear(localMeshColor, getGamma());
                }

                float localFOV = camera.getFovY() * 180.0f / M_PI;
                if (ImGui::SliderFloat("FOV", &localFOV, 30, 160)) {
                    camera.setFovY(localFOV * M_PI / 180.0f);
                }
                ImGui::SameLine();
                ImGui::HelpMarker("Vertical field of view in degrees");
                ImGui::Spacing();
            }

            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Lighting")) {
                ImGui::SliderFloat3("Light position", light.position.values, -2, 2);
                vec3 localLightColor = toGamma(light.color, getGamma());
                if (ImGui::ColorEdit3("Light color", localLightColor.values, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoAlpha)) {
                    light.color = toLinear(localLightColor, getGamma());
                }

                ImGui::SliderFloat("Light intensity", &light.intensity, 0, 10);

                if (ImGui::RadioButton("Phong shading", perPixelLighting)) {
                    perPixelLighting = true;
                }
                ImGui::SameLine();
                ImGui::HelpMarker("Computes the lighting for each pixel");

                ImGui::SameLine();
                if (ImGui::RadioButton("Flat shading", !perPixelLighting)) {
                    perPixelLighting = false;
                }

                ImGui::SameLine();
                ImGui::HelpMarker("Computes one value for each triangle using Monte Carlo");
                ImGui::Spacing();

                if (!perPixelLighting) {
                    ImGui::SliderInt("Samples", &samples, 1, 10000);
                    ImGui::SameLine();
                    ImGui::HelpMarker("Number of samples for flat shading with Monte Carlo");
                    ImGui::Spacing();

                    if (ImGui::Button("Recompute lighting")) {
                        computeLighting();
                    }
                }
            }

            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Postprocess")) {
                // Tonemapping is commented in TONEMAP_FRAG (glfw_hdr_application.cpp)
//                float localExposure = getExposure();
//                if (ImGui::SliderFloat("Exposure", &localExposure, 0, 10)) {
//                    setExposure(localExposure);
//                }

                float localGamma = getGamma();
                if (ImGui::SliderFloat("Gamma", &localGamma, 0.01, 2.2)) {
                    setGamma(localGamma);
                }
            }

            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Help")) {
                ImGui::Text("USER GUIDE:");
                ImGui::ShowUserGuide();
            }
        }

        // Render
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void changeCurrentModel(const int index) {
        if (modelIndex != index) {
            modelIndex = index;
            if (modelIndex != -1) {
                loadMesh(models.at(index));
            } else {
                cleanMesh();
            }
        }
    }

public:
    HdrApp() = default;

    ~HdrApp() {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void init(const std::map<int, int>& glfwWindowHints, const bool vsync = false) override final {
//        GLFWHDRApplication::onFramebufferSize(768, 768);
        GLFWHDRApplication::init(glfwWindowHints, vsync);
        setApplicationName("LightCalculator");
        setGamma(getGamma());

        // Setup model paths
        models.emplace_back("resources/models/box.obj");
        models.emplace_back("resources/models/box_tessellated.obj");
        models.emplace_back("resources/models/output.obj");
        models.emplace_back("resources/models/plane_tessellated.obj");
        models.emplace_back("resources/models/venus.obj");

        // Load mesh shader
        meshProgram.compileShader(load(R"(resources/shaders/mesh/mesh.frag)"), GLShaderType::FRAGMENT);
        meshProgram.compileShader(load(R"(resources/shaders/mesh/mesh.vert)"), GLShaderType::VERTEX);
        meshProgram.link();
        meshProgram.use();
        meshModelLoc = glGetUniformLocation(meshProgram.getProgramId(), "model");
        meshModelViewProjLoc = glGetUniformLocation(meshProgram.getProgramId(), "modelViewProjection");
        meshEyeLoc = glGetUniformLocation(meshProgram.getProgramId(), "eye");
        meshMeshColorLoc = glGetUniformLocation(meshProgram.getProgramId(), "meshColor");
        meshLightLoc = glGetUniformLocation(meshProgram.getProgramId(), "light");
        meshLightColorLoc = glGetUniformLocation(meshProgram.getProgramId(), "lightColor");
        meshLightIntensityLoc = glGetUniformLocation(meshProgram.getProgramId(), "lightIntensity");
        meshPerPixelLoc = glGetUniformLocation(meshProgram.getProgramId(), "perPixel");

        // Load light shader
        lightProgram.compileShader(load(R"(resources/shaders/light/light.frag)"), GLShaderType::FRAGMENT);
        lightProgram.compileShader(load(R"(resources/shaders/light/light.vert)"), GLShaderType::VERTEX);
        lightProgram.link();
        lightProgram.use();
        lightModelLoc = glGetUniformLocation(lightProgram.getProgramId(), "model");
        lightModelViewProjLoc = glGetUniformLocation(lightProgram.getProgramId(), "modelViewProjection");
        lightEyeLoc = glGetUniformLocation(lightProgram.getProgramId(), "eye");
        lightColorLoc = glGetUniformLocation(lightProgram.getProgramId(), "color");

        // Setup the scene
        setupScene();

        // OpenGL settings
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Init imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
//        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer bindings
        // GL 3.2 + GLSL 150
        const char *glsl_version = "#version 150";
        ImGui_ImplGlfw_InitForOpenGL(getWindow(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Don't transfer data ownership to imgui
        ImFontConfig fntConfig;
        fntConfig.FontDataOwnedByAtlas = false;
        cmrc::file fntUIFile = fs.open(R"(resources/fonts/Roboto-Regular.ttf)");
        io.Fonts->AddFontFromMemoryTTF((char *)fntUIFile.begin(), fntUIFile.size(), 24.0f, &fntConfig);

        // Setup the viewport
        updateViewport(getViewportWidth(), getViewportHeight());
    }

    void setGamma(const float value) override {
        GLFWHDRApplication::setGamma(value);

        // Adjust background color
        const vec3 backgroundColor = toLinear(vec3(60_rgbf, 60_rgbf, 60_rgbf), value);
        glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
    }

    static int main(int argc, char* argv[]) {
        HdrApp app;
        try {
            app.init(windowHints, true);
            app.run();
            return EXIT_SUCCESS;
        } catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
            return EXIT_FAILURE;
        }
    }
};

int main(int argc, char *argv[]) {
#ifdef CHECK_LEAKS
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // Send all reports to STDOUT
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif

    return HdrApp::main(argc, argv);
}
