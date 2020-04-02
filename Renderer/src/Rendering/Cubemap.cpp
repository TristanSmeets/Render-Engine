#include "Rendererpch.h"
#include "Cubemap.h"

Cubemap::Cubemap(GLsizei width, GLsizei height, GLuint internalFormat, GLenum format, GLenum type) :
	width(width), height(height)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width,height, 0, format, type, nullptr);
	}

	SetTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	SetTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	SetTextureParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Cubemap::~Cubemap()
{
}

void Cubemap::SetTextureParameter(GLenum name, GLint value)
{
	glTexParameteri(GL_TEXTURE_CUBE_MAP, name, value);
}

GLuint Cubemap::GetID() const
{
	return id;
}

const GLsizei Cubemap::GetWidth() const
{
	return width;
}

const GLsizei Cubemap::GetHeight() const
{
	return height;
}
