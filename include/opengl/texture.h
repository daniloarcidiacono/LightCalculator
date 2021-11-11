#pragma once
#include <opengl/includes.h>
#include <functional>
#include <cassert>

struct GLTextureFormat {
public:
	static const GLTextureFormat NONE;
    static const GLTextureFormat RGB;
    static const GLTextureFormat RGBA;
	static const GLTextureFormat RGB32F;
	static const GLTextureFormat RGBA32F;
	static const GLTextureFormat DEPTHF;

	const GLint internalFormat;
	const GLenum format;
	const GLenum type;

	bool operator <(const GLTextureFormat& other) const {
		if (internalFormat == other.internalFormat) {
			if (format == other.format) {
				return type < other.type;
			}

			return format < other.format;
		}

		return internalFormat < other.internalFormat;
	}

private:
	GLTextureFormat(const GLint internalFormat, const GLenum format, const GLenum type)
		: internalFormat(internalFormat), format(format), type(type) {
	}
};

class GLTexture {
private:
	GLuint textureId;
	int width, height;
	std::reference_wrapper<const GLTextureFormat> format;

	void reset();

public:
	GLTexture();
	~GLTexture();

    // Disable copying
    GLTexture(const GLTexture &other) = delete;
    GLTexture &operator =(const GLTexture &other) = delete;

    GLTexture &create(const int width, const int height, const GLTextureFormat &format);
	GLTexture &resize(const int width, const int height);
    GLTexture &update(void *pixels);
    GLTexture &read(void *pixels);
	bool isBound() const;
	GLTexture& bind(const GLint textureUnit);
	GLTexture& unbind();
	GLTexture& setTextureMinFilter(const GLint filter);
	GLTexture& setTextureMagFilter(const GLint filter);
	GLTexture& setTextureFilters(const GLint minFilter, const GLint magFilter);
    GLTexture& setTextureSWrap(const GLint wrap);
	GLTexture& setTextureTWrap(const GLint wrap);
	GLTexture& setTextureWraps(const GLint sWrap, const GLint tWrap);

	void destroy();

	// Getters
	GLuint getTextureId() const { return textureId; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	const GLTextureFormat& getFormat() const { return format; }
}; 

inline GLTexture& GLTexture::bind(const GLint textureUnit) {
	assert(textureUnit >= GL_TEXTURE0 && textureUnit <= GL_TEXTURE31);
	assert(textureId != 0);
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureId);
	return *this;
}

inline GLTexture& GLTexture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
	return *this;
}

inline GLTexture& GLTexture::setTextureMinFilter(const GLint filter) {
	assert(isBound());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	return *this;
}

inline GLTexture& GLTexture::setTextureMagFilter(const GLint filter) {
	assert(isBound());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	return *this;
}

inline GLTexture& GLTexture::setTextureFilters(const GLint minFilter, const GLint magFilter) {
	return setTextureMinFilter(minFilter).setTextureMagFilter(magFilter);
}

inline GLTexture & GLTexture::setTextureSWrap(const GLint wrap) {
    assert(isBound());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    return *this;
}

inline GLTexture & GLTexture::setTextureTWrap(const GLint wrap) {
    assert(isBound());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    return *this;
}

inline GLTexture & GLTexture::setTextureWraps(const GLint sWrap, const GLint tWrap) {
    return setTextureSWrap(sWrap).setTextureTWrap(tWrap);
}

