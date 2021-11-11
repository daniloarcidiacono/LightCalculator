#pragma once
#include <opengl/shader.h>
#include <map>

namespace std {
	// https://stackoverflow.com/questions/3235927/reference-as-key-in-stdmap
	// https://stackoverflow.com/questions/9139748/using-stdreference-wrapper-as-the-key-in-a-stdmap
	template<class K, class V>
	using refmap = std::map<std::reference_wrapper<K>, V, std::less<K>>;
};

class GLProgram {
public:
//    struct Attribute {
//        std::string name;
//        GLint size;
//        GLenum type;
//        GLint location;
//
//        Attribute() {
//        }
//
//        Attribute(const std::string &name, const GLint size, const GLenum type, const GLint location)
//            : name(name), size(size), type(type), location(location) {
//        }
//    };

private:
	std::refmap<const GLShaderType, GLShader *> shaders;
	GLuint programId;
	std::string name;
	bool linked;

	void reset();

public:
	GLProgram(const std::string &name = "");
	~GLProgram();

	// Disable copying
    GLProgram(const GLProgram &other) = delete;
    GLProgram &operator =(const GLProgram &other) = delete;

	void compileShader(const std::string& source, const GLShaderType &type);
	void link();
	void use() const;
	std::string getInfoLog() const;

	void destroy();

	// Getters	
	const std::string& getName() const { return name; }
	GLuint getProgramId() const { return programId; }	
	bool isLinked() const { return linked; }
	GLShader *getShader(const GLShaderType& type) const {
		auto it = shaders.find(type);
		return it != shaders.end() ? (*it).second : nullptr;
	}
};
