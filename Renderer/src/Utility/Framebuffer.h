#pragma once

#include "glad/glad.h"
#include "Rendering/Texture.h"
#include "Utility/Renderbuffer.h"
#include "Utility/Cubemap.h"

class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();
	void Bind();
	void Unbind();
	void AttachTexture(const Texture& texture);
	void AttachRenderbuffer(GLenum attachment, const Renderbuffer& buffer);
	void AttachCubemap(const Cubemap& cubemap);
	GLuint GetBuffer() const;

private:
	void AttachTexture(GLenum texTarget, GLuint texture);
	GLuint buffer;
};
