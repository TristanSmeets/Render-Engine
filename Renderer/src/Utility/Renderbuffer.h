#pragma once

#include "glad/glad.h"
#include "glm.hpp"

class Renderbuffer
{
public:
	struct MultiSampleParameters
	{
		GLsizei Samples;
		GLenum InternalFormat;
		glm::ivec2 Resolution;
	};
public:
	Renderbuffer();
	~Renderbuffer();
	void Generate();
	void Bind() const;
	void Unbind() const;
	void SetStorage(GLenum format, GLsizei width, GLsizei height);
	void SetStorageMultiSample(const MultiSampleParameters& parameters);
	GLuint GetID() const;
	GLsizei GetWidth() const;
	GLsizei GetHeight() const;

private:
	GLuint id;
	GLsizei width = 0;
	GLsizei height = 0;
};