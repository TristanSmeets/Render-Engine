#include "Rendererpch.h"
#include "Framebuffer.h"

Framebuffer::Framebuffer()
{
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &buffer);
}

void Framebuffer::Generate()
{
	glGenFramebuffers(1, &buffer);
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

void Framebuffer::AttachTexture(GLenum attachement, GLenum texTarget, GLuint texture)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachement, texTarget, texture, 0);
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

void Framebuffer::BlitFramebuffer(const Framebuffer::BlitParameters& parameters)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, parameters.Destination->GetBuffer());
	glBlitFramebuffer(0, 0, parameters.Resolution.x, parameters.Resolution.y, 0, 0, parameters.Resolution.x, parameters.Resolution.y, parameters.Mask, parameters.Filter);
}

bool Framebuffer::IsCompleted()
{
	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

GLuint Framebuffer::GetBuffer() const
{
	return buffer;
}

void Framebuffer::AttachTexture(GLenum texTarget, GLuint texture)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texTarget, texture, 0);
}
