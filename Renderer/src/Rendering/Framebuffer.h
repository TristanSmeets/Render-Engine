#pragma once

#include "glad/glad.h"
#include "Rendering/Texture.h"
#include "Rendering/Renderbuffer.h"

class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();
	void Bind();
	void Unbind();
	void AttachTexture(const Texture& texture);
	void AttachRenderbuffer(GLenum attachment, const Renderbuffer& buffer);
	GLuint GetBuffer() const;

private:
	GLuint buffer;
};
