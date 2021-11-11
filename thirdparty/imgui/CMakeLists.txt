set(
    SRCS
        src/imgui.cpp
        src/imgui_demo.cpp
        src/imgui_draw.cpp
        src/imgui_impl_glfw.cpp
        src/imgui_impl_opengl3.cpp
        src/imgui_widgets.cpp
)

set(
    INCLUDES
        include/imgui/imconfig.h
        include/imgui/imgui.h
        include/imgui/imgui_impl_glfw.h
        include/imgui/imgui_impl_opengl3.h
        include/imgui/imgui_internal.h
        include/imgui/imstb_rectpack.h
        include/imgui/imstb_textedit.h
        include/imgui/imstb_truetype.h
        include/imgui/includes.h
)

# Define the library
add_library(imgui STATIC ${SRCS} ${INCLUDES})

# Allow includes from include/
target_include_directories(
    imgui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set_target_properties(
    imgui
    PROPERTIES 
        DEBUG_POSTFIX "_d"
        PUBLIC_HEADER "${INCLUDES}"
        LINKER_LANGUAGE CXX
)

target_compile_features(
    imgui
    PUBLIC
        cxx_std_11
)

# glfw3
find_package(glfw3 REQUIRED)
target_link_libraries(imgui PUBLIC glfw)
add_custom_command(
    TARGET imgui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:glfw>
    $<TARGET_FILE_DIR:imgui>
)

# GLEW
find_package(GLEW REQUIRED CONFIG)
target_link_libraries(imgui PUBLIC GLEW::glew)
add_custom_command(
    TARGET imgui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:GLEW::glew>
    $<TARGET_FILE_DIR:imgui>
)
