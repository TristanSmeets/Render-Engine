#include "Rendererpch.h"
#include "Renderbuffer.h"

Renderbuffer::Renderbuffer()
{
}

Renderbuffer::~Renderbuffer()
{
	glDeleteRenderbuffers(1, &id);
}

void Renderbuffer::Generate()
{
	glGenRenderbuffers(1, &id);
}

void Renderbuffer::Bind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, id);
}

void Renderbuffer::Unbind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Renderbuffer::SetStorage(GLenum format, GLsizei width, GLsizei height)
{
	this->width = width;
	this->height = height;

	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
}

void Renderbuffer::SetStorageMultiSample(const MultiSampleParameters & parameters)
{
	this->width = parameters.Resolution.x;
	this->height = parameters.Resolution.y;

	glRenderbufferStorageMultisample(GL_RENDERBUFFER, parameters.Samples, parameters.InternalFormat, width, height);
}

GLuint Renderbuffer::GetID() const
{
	return id;
}

GLsizei Renderbuffer::GetWidth() const
{
	return width;
}

GLsizei Renderbuffer::GetHeight() const
{
	return height;
}
