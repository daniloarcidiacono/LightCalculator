#include <opengl/program.h>
#include <iostream>
#include <stdexcept>

GLProgram::GLProgram(const std::string &name)
    : name(name) {    
    reset();
}

GLProgram::~GLProgram() {
    destroy();
}

void GLProgram::reset() {
    shaders.clear();
    programId = 0;
    linked = false;
}

void GLProgram::destroy() {
    for (const auto& entry : shaders) {
        if (programId != 0) {
            glDetachShader(programId, entry.second->getShaderId());
        }

        delete entry.second;
    }

    if (programId != 0) {
        glDeleteProgram(programId);
    }

    reset();
}

void GLProgram::compileShader(const std::string& source, const GLShaderType &type) {
    if (shaders.find(type) != shaders.end()) {
        delete shaders[type];
        shaders.erase(type);
    }

    GLShader* shader = new GLShader();
    shader->compile(source, type);
    shaders[type] = shader;
}

void GLProgram::link() {
    programId = glCreateProgram();
    glObjectLabel(GL_PROGRAM, programId, -1, name.c_str());
    for (const auto& entry : shaders) {
        glAttachShader(programId, entry.second->getShaderId());
    }

    glLinkProgram(programId);

    // Check for link status
    GLint glLinked = 0;
    glGetProgramiv(programId, GL_LINK_STATUS, &glLinked);
    linked = (glLinked == GL_TRUE);
    if (!linked) {
        throw std::runtime_error("Could not link" + getInfoLog());
    }

    // Extract active attributes
//    GLint attributes = 0;
//    glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTES, &attributes);
//    char attributeName[1024];
//    GLsizei attributeNameLength;
//    for (int i = 0; i < attributes; i++) {
//        Attribute attribute;
//        glGetActiveAttrib(
//            programId,
//            i,
//            sizeof(attributeName),
//            &attributeNameLength,
//            &attribute.size,
//            &attribute.type,
//            attributeName
//        );
//        attribute.location = glGetAttribLocation(programId, attributeName);
//        attribute.name = std::string(attributeName);
//    }
}

void GLProgram::use() const {
    glUseProgram(programId);
}

std::string GLProgram::getInfoLog() const {
    int infoLogLength = 0;
    if (programId != 0) {
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
    }

    if (infoLogLength > 0) {
        GLchar* buffer = new GLchar[infoLogLength];
        int charsWritten = 0;
        glGetProgramInfoLog(programId, infoLogLength, &charsWritten, buffer);
        const std::string result(buffer);
        delete []buffer;

        return result;
    }

    return "";
}
