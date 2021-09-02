#pragma once

#include "Utility/Framebuffer.h"
#include "Utility/Renderbuffer.h"
#include "Utility/NDCQuad.h"
#include "Core/Window.h"
#include "Rendering/GLSLProgram.h"
#include "Rendering/Texture.h"
#include "Utility/Filepath.h"

class PostProcessing
{
public:
	PostProcessing();
	virtual ~PostProcessing();
	virtual void Initialize(const Window::Parameters& parameters);
	virtual void Bind();
	virtual void Unbind();
	virtual void Apply();
	virtual const Framebuffer& GetFramebuffer() const;
	virtual const Texture& GetTexture() const;

private:
	GLSLProgram program;
	Framebuffer framebuffer;
	Renderbuffer renderbuffer;
	Texture colourAttachment;
	NDCQuad quad;
};
