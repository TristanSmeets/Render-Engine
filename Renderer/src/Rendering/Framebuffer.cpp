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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.GetID(), 0);
}

void Framebuffer::AttachRenderbuffer(GLenum attachment, const Renderbuffer & buffer)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, buffer.GetID());
}

GLuint Framebuffer::GetBuffer() const
{
	return buffer;
}
