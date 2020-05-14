#pragma once

#include "glad/glad.h"
#include "Rendering/Texture.h"
#include "Utility/Renderbuffer.h"
#include "Utility/Cubemap.h"

class Framebuffer
{
public:
	struct BlitParameters
	{
		const Framebuffer* Destination;
		glm::ivec2 Resolution;
		GLbitfield Mask;
		GLenum Filter;
	};
public:
	Framebuffer();
	~Framebuffer();
	void Generate();
	void Bind();
	void Unbind();
	void AttachTexture(const Texture& texture);
	void AttachTexture(GLenum attachement, GLenum texTarget, GLuint texture);
	void AttachRenderbuffer(GLenum attachment, const Renderbuffer& buffer);
	void AttachCubemap(const Cubemap& cubemap);
	void BlitFramebuffer(const BlitParameters& parameters);
	bool IsCompleted();
	GLuint GetBuffer() const;

private:
	void AttachTexture(GLenum texTarget, GLuint texture);
	GLuint buffer;
};
