#include <opengl/vao.h>
#include <iostream>
#include <assert.h>

const GLVertexAttributeFormat GLVertexAttributeFormat::NONE(0, GL_FALSE, GL_FALSE);
const GLVertexAttributeFormat GLVertexAttributeFormat::SCALAR1I(1, GL_INT, GL_FALSE);
const GLVertexAttributeFormat GLVertexAttributeFormat::SCALAR1F(1, GL_FLOAT, GL_FALSE);
const GLVertexAttributeFormat GLVertexAttributeFormat::SCALAR4I(4, GL_INT, GL_FALSE);
const GLVertexAttributeFormat GLVertexAttributeFormat::SCALAR4F(4, GL_FLOAT, GL_FALSE);
const GLVertexAttributeFormat GLVertexAttributeFormat::POS2F(2, GL_FLOAT, GL_FALSE);
const GLVertexAttributeFormat GLVertexAttributeFormat::POS3F(3, GL_FLOAT, GL_FALSE);
const GLVertexAttributeFormat GLVertexAttributeFormat::NORMAL3F(3, GL_FLOAT, GL_TRUE);
const GLVertexAttributeFormat GLVertexAttributeFormat::COL3F(3, GL_FLOAT, GL_FALSE);
const GLVertexAttributeFormat GLVertexAttributeFormat::TEXCOORD2F(2, GL_FLOAT, GL_FALSE);

GLVao::GLVao() {
	reset();
}

GLVao::~GLVao() {
	destroy();
}

void GLVao::reset() {	
	vaoId = 0;
}

void GLVao::destroy() {
	if (vaoId != 0) {
		glDeleteVertexArrays(1, &vaoId);
	}

	reset();
}

GLVao &GLVao::create() {
	if (vaoId != 0) {
		destroy();
	}

	glGenVertexArrays(1, &vaoId);
	return *this;
}

bool GLVao::isBound() const {
	GLint currentVao;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVao);
	return vaoId == currentVao;
}
