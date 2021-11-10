cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# Global variables (this will be seen by modules)
set(PACKAGE_VERSION 0.1)
set(PACKAGE_VERSION_VARIANT SNAPSHOT)
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

# Create module target
set(
    MODULE_INCLUDES
        include/vec3.h
        include/triangle.h
)

set(
    MODULE_SRCS
        src/main.cpp
)

# Create main target
add_executable(${MODULE_TARGET} ${MODULE_INCLUDES} ${MODULE_SRCS})
target_compile_features(
    ${MODULE_TARGET}
    PUBLIC
        cxx_std_11
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
