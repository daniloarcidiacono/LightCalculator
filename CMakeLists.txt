cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
set(PACKAGE_VERSION 0.1)
set(MODULE_TARGET LightCalculator)

# Define the root project
project(
    LightCalculator
        VERSION ${PACKAGE_VERSION}
        DESCRIPTION "Light calculator"
        LANGUAGES CXX
)

# Let's nicely support folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Third party libs
add_subdirectory(thirdparty)

# Create module target
set(
    MODULE_INCLUDES
        include/glfw/glfw_application.h
        include/glfw/glfw_hdr_application.h
        include/math/includes.h
        include/math/light.h
        include/math/mat4.h
        include/math/spherical.h
        include/math/triangle.h
        include/math/uber_camera.h
        include/math/vec3.h
        include/opengl/includes.h
        include/opengl/program.h
        include/opengl/shader.h
        include/opengl/texture.h
        include/opengl/vao.h
        include/opengl/vbo.h
)

set(
    MODULE_SRCS
        src/glfw/glfw_application.cpp
        src/glfw/glfw_hdr_application.cpp
        src/math/uber_camera.cpp
        src/opengl/program.cpp
        src/opengl/shader.cpp
        src/opengl/texture.cpp
        src/opengl/vao.cpp
        src/main.cpp
)

set(
    MODULE_RESOURCES
        resources/fonts/Roboto-Regular.ttf
        resources/models/box.obj
        resources/models/box_tessellated.obj
        resources/models/light.obj
        resources/models/output.obj
        resources/models/plane_tessellated.obj
        resources/models/venus.obj
        resources/shaders/mesh/mesh.frag
        resources/shaders/mesh/mesh.vert
        resources/shaders/light/light.frag
        resources/shaders/light/light.vert
)

# Bundle resources
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CMakeRC.cmake")
cmrc_add_resource_library(
    LightCalculatorRC
    ALIAS LightCalculator::rc
    NAMESPACE LightCalculator
    ${MODULE_RESOURCES}
)

# Create main target
add_executable(${MODULE_TARGET} ${MODULE_INCLUDES} ${MODULE_SRCS})
target_compile_features(
    ${MODULE_TARGET}
    PUBLIC
        cxx_std_17
)

target_include_directories(
    ${MODULE_TARGET}
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set_target_properties(
    ${MODULE_TARGET}
    PROPERTIES
        DEBUG_POSTFIX "d"
        PUBLIC_HEADER "${MODULE_INCLUDES}"
)

# Resources
target_link_libraries(${MODULE_TARGET} PUBLIC LightCalculator::rc)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(${MODULE_TARGET} PUBLIC OpenGL::GL)

# glfw3
find_package(glfw3 REQUIRED)
target_link_libraries(${MODULE_TARGET} PUBLIC glfw)
add_custom_command(
    TARGET ${MODULE_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:glfw>
    $<TARGET_FILE_DIR:${MODULE_TARGET}>
)

# GLEW
find_package(GLEW REQUIRED CONFIG)
target_link_libraries(${MODULE_TARGET} PUBLIC GLEW::glew)
add_custom_command(
    TARGET ${MODULE_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:GLEW::glew>
    $<TARGET_FILE_DIR:${MODULE_TARGET}>
)

# TinyObjLoader
target_link_libraries(${MODULE_TARGET} PUBLIC tiny-obj-loader)

# Dear Imgui
target_link_libraries(${MODULE_TARGET} PUBLIC imgui)