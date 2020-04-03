#pragma once

#include "glad/glad.h"

class Renderbuffer
{
public:
	Renderbuffer();
	~Renderbuffer();
	void Bind() const;
	void Unbind() const;
	void SetStorage(GLenum format, GLsizei width, GLsizei height);
	GLuint GetID() const;
	GLsizei GetWidth() const;
	GLsizei GetHeight() const;

private:
	GLuint id;
	GLsizei width = 0;
	GLsizei height = 0;
};