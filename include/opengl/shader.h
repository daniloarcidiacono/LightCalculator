#pragma once
#include <opengl/includes.h>
#include <string>

struct GLShaderType {
public:
	static const GLShaderType NONE;
	static const GLShaderType VERTEX;
	static const GLShaderType FRAGMENT;
	static const GLShaderType COMPUTE;

	const std::string name;
	const GLuint glType;

	bool operator <(const GLShaderType& other) const {
		return glType < other.glType;
	}

private:
	GLShaderType(const std::string& name, const GLuint glType)
		: name(name), glType(glType) {
	}
};

class GLShader {
private:
	std::reference_wrapper<const GLShaderType> type;
	GLuint shaderId;
	bool compiled;

	void reset();

public:
	GLShader();
	~GLShader();

    // Disable copying
    GLShader(const GLShader &other) = delete;
    GLShader &operator =(const GLShader &other) = delete;

    void compile(const std::string& source, const GLShaderType& type);
	std::string getInfoLog() const;

	void destroy();

	// Getters
	const GLShaderType& getType() const { return type; }
	GLuint getShaderId() const { return shaderId; }
	bool isCompiled() const { return compiled; }
};
