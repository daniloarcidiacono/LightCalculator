#include <opengl/shader.h>
#include <iostream>
#include <stdexcept>

const GLShaderType GLShaderType::NONE("None", 0);
const GLShaderType GLShaderType::VERTEX("Vertex", GL_VERTEX_SHADER);
const GLShaderType GLShaderType::FRAGMENT("Fragment", GL_FRAGMENT_SHADER);
const GLShaderType GLShaderType::COMPUTE("Compute", GL_COMPUTE_SHADER);

GLShader::GLShader() 
    : type(GLShaderType::NONE) {
    reset();
}

GLShader::~GLShader() {
    destroy();    
}

void GLShader::reset() {
    type = std::cref(GLShaderType::NONE);
    shaderId = 0;
    compiled = false;
}

void GLShader::destroy() {
    if (shaderId != 0) {
        glDeleteShader(shaderId);
    }

    reset();
}

void GLShader::compile(const std::string& source, const GLShaderType& type) {
    shaderId = glCreateShader(type.glType);
    this->type = std::cref(type);
    glObjectLabel(GL_SHADER, shaderId, -1, type.name.c_str());

    const char* sourceStr = source.c_str();
    glShaderSource(shaderId, 1, &sourceStr, NULL);
    glCompileShader(shaderId);

    GLint glCompiled;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &glCompiled);
    compiled = (glCompiled == GL_TRUE);
    if (!compiled) {
        throw std::runtime_error(std::string("Could not compile: ") + getInfoLog());
    }    
}

std::string GLShader::getInfoLog() const {
    int infoLogLength = 0;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        GLchar* buffer = new GLchar[infoLogLength];
        int charsWritten = 0;
        glGetShaderInfoLog(shaderId, infoLogLength, &charsWritten, buffer);
        const std::string result(buffer);
        delete[]buffer;

        return result;
    }

    return "";
}
