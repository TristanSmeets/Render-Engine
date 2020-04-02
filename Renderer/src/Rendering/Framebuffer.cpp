#include "Rendererpch.h"
#include "Framebuffer.h"

Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &buffer);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &buffer);
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, buffer);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::AttachTexture(const Texture & texture)
{
	AttachTexture(GL_TEXTURE_2D, texture.GetID());
}

void Framebuffer::AttachRenderbuffer(GLenum attachment, const Renderbuffer & buffer)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, buffer.GetID());
}

void Framebuffer::AttachCubemap(const Cubemap & cubemap)
{
	for (unsigned int i = 0; i < 6; ++i);
	{
		AttachTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1, cubemap.GetID());
	}
}

GLuint Framebuffer::GetBuffer() const
{
	return buffer;
}

void Framebuffer::AttachTexture(GLenum texTarget, GLuint texture)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texTarget, texture, 0);
}
