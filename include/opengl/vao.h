#pragma once
#include <opengl/vbo.h>

struct GLVertexAttributeFormat {
public:
	static const GLVertexAttributeFormat NONE;
	static const GLVertexAttributeFormat SCALAR1I;
	static const GLVertexAttributeFormat SCALAR1F;
	static const GLVertexAttributeFormat SCALAR4I;
	static const GLVertexAttributeFormat SCALAR4F;
	static const GLVertexAttributeFormat POS2F;
	static const GLVertexAttributeFormat POS3F;
	static const GLVertexAttributeFormat NORMAL3F;
	static const GLVertexAttributeFormat COL3F;
	static const GLVertexAttributeFormat TEXCOORD2F;

	GLint size;
	GLenum type;
	GLboolean normalized;

    GLVertexAttributeFormat()
        : size(NONE.size), type(NONE.type), normalized(NONE.normalized) {
	}

	GLVertexAttributeFormat(const GLint size, const GLenum type, const GLboolean normalized)
		: size(size), type(type), normalized(normalized) {
	}

    bool operator <(const GLVertexAttributeFormat& other) const {
        if (size == other.size) {
            if (type == other.type) {
                return normalized < other.normalized;
            }

            return type < other.type;
        }

        return size < other.size;
    }
};

class GLVao {
private:
	GLuint vaoId;

	void reset();

public:
	GLVao();
	~GLVao();

    // Disable copying
    GLVao(const GLVao &other) = delete;
    GLVao &operator =(const GLVao &other) = delete;

	GLVao& create();
	bool isBound() const;
	GLVao& bind();
	GLVao& unbind();
	GLVao& vertexAttribPointer(const GLuint index, const GLVertexAttributeFormat& vertexFormat, const GLuint offset, const GLVbo &vbo);

	void destroy();

	// Getters
	GLuint getVaoId() const { return vaoId; }
};

inline GLVao& GLVao::bind() {
	glBindVertexArray(vaoId);
	return *this;
}

inline GLVao& GLVao::unbind() {
	glBindVertexArray(0);
	return *this;
}

inline GLVao& GLVao::vertexAttribPointer(const GLuint index, const GLVertexAttributeFormat& vertexFormat, const GLuint offset, const GLVbo& vbo) {
	assert(isBound());
	assert(vbo.isBound());

	glEnableVertexAttribArray(index);
	if (vertexFormat.type == GL_INT) {
        glVertexAttribIPointer(index, vertexFormat.size, vertexFormat.type, vbo.getElementSize(), reinterpret_cast<void *>(offset));
    } else {
        glVertexAttribPointer(index, vertexFormat.size, vertexFormat.type, vertexFormat.normalized, vbo.getElementSize(), reinterpret_cast<void *>(offset));
	}

	return *this;
}
