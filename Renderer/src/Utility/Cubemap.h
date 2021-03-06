#pragma once

#include "glad/glad.h"

class Cubemap
{
public:
	Cubemap();
	~Cubemap();
	Cubemap(const Cubemap& rhs);

	void CreateTexture(GLsizei width, GLsizei height, GLuint internalFormat, GLenum format, GLenum type);
	void SetTextureParameter(GLenum name, GLint value);
	const GLuint& GetID() const;
	const GLsizei GetWidth() const;
	const GLsizei GetHeight() const;
	void Bind() const;

private:
	GLuint id = 0;
	GLsizei width = 0;
	GLsizei height = 0;
};
