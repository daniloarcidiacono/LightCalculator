#include <opengl/texture.h>
#include <iostream>

#ifdef RGB
#undef RGB
#endif

const GLTextureFormat GLTextureFormat::NONE(0, 0, 0);
const GLTextureFormat GLTextureFormat::RGB(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
const GLTextureFormat GLTextureFormat::RGBA(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
const GLTextureFormat GLTextureFormat::RGB32F(GL_RGB32F, GL_RGB, GL_FLOAT);
const GLTextureFormat GLTextureFormat::RGBA32F(GL_RGBA32F, GL_RGBA, GL_FLOAT);
const GLTextureFormat GLTextureFormat::DEPTHF(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);

GLTexture::GLTexture()
    : format(GLTextureFormat::NONE) {
	reset();
}

GLTexture::~GLTexture() {
	destroy();
}

void GLTexture::reset() {
    textureId = 0;
    width = height = 0;
    format = GLTextureFormat::NONE;
}

void GLTexture::destroy() {
    if (textureId != 0) {
        // A texture binding created with glBindTexture remains active until a different texture 
        // is bound to the same target, or until the bound texture is deleted with glDeleteTextures.
        glDeleteTextures(1, &textureId);
    }

    reset();
}

bool GLTexture::isBound() const {
    GLint currentTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
    return textureId == currentTexture;
}

GLTexture &GLTexture::create(const int width, const int height, const GLTextureFormat &format) {
    if (textureId != 0) {
        destroy();
    }

    // Setup the new format
    this->format = format;

    // Generate the texture
    glGenTextures(1, &textureId);

    // Resize to the desired size
    return resize(width, height);
}

GLTexture& GLTexture::resize(const int width, const int height) {
    this->width = width;
    this->height = height;

    // Get old texture of current active texture unit
    GLint oldTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTexture);

    // Bind the new texture to the current active texture unit
    glBindTexture(GL_TEXTURE_2D, textureId);

    // glTexImage2D specifies the two-dimensional texture for the current texture unit
    glTexImage2D(GL_TEXTURE_2D, 0, format.get().internalFormat, width, height, 0, format.get().format, format.get().type, 0);

    // Restore the old texture in the current active texture unit
    glBindTexture(GL_TEXTURE_2D, oldTexture);

    return *this;
}

GLTexture &GLTexture::update(void *pixels) {
    // Get old texture of current active texture unit
    GLint oldTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTexture);

    // Bind the new texture to the current active texture unit
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Update the texture
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format.get().format, format.get().type, (void *)pixels);

    // Restore the old texture in the current active texture unit
    glBindTexture(GL_TEXTURE_2D, oldTexture);

    return *this;
}

GLTexture &GLTexture::read(void *pixels) {
    // Get old texture of current active texture unit
    GLint oldTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTexture);

    // Bind the new texture to the current active texture unit
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Update the texture
    glGetTexImage(GL_TEXTURE_2D, 0, format.get().format, format.get().type, pixels);

    // Restore the old texture in the current active texture unit
    glBindTexture(GL_TEXTURE_2D, oldTexture);

    return *this;
}
