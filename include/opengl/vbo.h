#pragma once
#include <opengl/includes.h>
#include <initializer_list>
#include <iostream>
#include <cassert>

class GLVbo {
private:
	GLuint vboId;
	GLsizei elementSize;

	void reset();

public:
	GLVbo() {
		reset();
	}

	~GLVbo() {
		destroy();
	}

	// Disable copying
	GLVbo(const GLVbo &other) = delete;
	GLVbo &operator =(const GLVbo &other) = delete;

	template<class T> GLVbo &create(const GLenum usage, const int count, const T *data = nullptr);
    template<class T> GLVbo &create(const GLenum usage, std::initializer_list<T> data);
	bool isBound() const;
	const GLVbo &bind() const;
	const GLVbo &unbind() const;
    template<class T> GLVbo &update(const T* data, const int count, const int start = 0);
    template<class T> GLVbo &update(std::initializer_list<T> data, const int start = 0);

	void destroy();

	// Getters
	GLuint getVboId() const { return vboId; }
    GLsizei getElementSize() const { return elementSize; }

    // Setters
    GLVbo &setElementSize(GLsizei elementSize) { this->elementSize = elementSize; return *this; }
};

inline void GLVbo::reset() {
	vboId = 0;
	elementSize = 0;
}

inline void GLVbo::destroy() {
	if (vboId != 0) {
		glDeleteBuffers(1, &vboId);
	}

	reset();
}

inline bool GLVbo::isBound() const {
	GLint currentBuffer;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBuffer);
	return vboId == currentBuffer;
}

inline const GLVbo& GLVbo::bind() const {
	assert(vboId != 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	return *this;
}

inline const GLVbo& GLVbo::unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return *this;
}

template<class T>
GLVbo& GLVbo::create(const GLenum usage, std::initializer_list<T> data) {
	// C++11 [support.initlist] 18.9 / 1 specifies that for std::initializer_list<T> iterator must be T*,
	// so it's guaranteed that sequential elements in a single initializer_list are contiguous.
	return create(usage, data.size(), data.begin());
}

template<class T>
GLVbo& GLVbo::create(const GLenum usage, const int count, const T* data) {
	assert(count > 0);
	if (vboId != 0) {
		destroy();
	}

	// Generate the buffer
	glGenBuffers(1, &vboId);

	// Get old array buffer binding
	GLint oldBuffer;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);

	// Bind the buffer and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(T), data, usage);

	// Restore the old array buffer binding
	glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);

	// Track the element size
	elementSize = sizeof(T);
	return *this;
}

template<class T>
inline GLVbo& GLVbo::update(const T* data, const int count, const int start) {
	assert(isBound());
	assert(sizeof(T) == elementSize);
	glBufferSubData(GL_ARRAY_BUFFER, start * sizeof(T), count * sizeof(T), data);
	return *this;
}

template<class T>
inline GLVbo& GLVbo::update(std::initializer_list<T> data, const int start) {
	// C++11 [support.initlist] 18.9 / 1 specifies that for std::initializer_list<T> iterator must be T*,
	// so it's guaranteed that sequential elements in a single initializer_list are contiguous.
	return update(data.begin(), data.size(), start);
}
