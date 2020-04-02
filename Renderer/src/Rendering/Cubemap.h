#pragma once

#include "glad/glad.h"

class Cubemap
{
public:
	Cubemap(GLsizei width, GLsizei height, GLuint internalFormat, GLenum format, GLenum type);
	~Cubemap();
	
	void SetTextureParameter(GLenum name, GLint value);
	GLuint GetID() const;
	const GLsizei GetWidth() const;
	const GLsizei GetHeight() const;

private:
	GLuint id = 0;
	GLsizei width;
	GLsizei height;
};
