#pragma once

#include "Utility/Framebuffer.h"
#include "Utility/Renderbuffer.h"
#include "Utility/NDCQuad.h"
#include "Core/Window.h"
#include "Rendering/Shader.h"

class PostProcessing
{
public:
	PostProcessing();
	virtual ~PostProcessing();
	virtual void Initialize(const Window::Parameters& parameters);
	virtual void Bind();
	virtual void Unbind();
	virtual void Draw();
private:
	Framebuffer framebuffer;
	Renderbuffer renderbuffer;
	Texture empty;
	NDCQuad quad;
	Shader shader;
};
