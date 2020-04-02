#pragma once

#include "glad/glad.h"
#include "Rendering/Texture.h"

class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();
	void Bind();
	void Unbind();
	void AttachTexture(const Texture& texture);
	GLuint GetBuffer() const;

private:
	GLuint buffer;
};
