#pragma once

#include "Utility/Framebuffer.h"
#include "Utility/Renderbuffer.h"
#include "Utility/NDCQuad.h"
#include "Core/Window.h"
#include "Rendering/Shader.h"
#include "Rendering/Texture.h"
#include "Utility/Filepath.h"

class PostProcessing
{
public:
	PostProcessing();
	virtual ~PostProcessing();
	virtual void Initialize(const Window::Parameters& parameters) = 0;
	virtual void Bind() = 0;
	virtual void Unbind() = 0;
	virtual void Draw() = 0;
	virtual const Framebuffer& GetFramebuffer() const = 0;
};
