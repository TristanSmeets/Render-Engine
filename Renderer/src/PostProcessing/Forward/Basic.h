#pragma once

#include "PostProcessing/PostProcessing.h"

class Basic : public PostProcessing
{
public:
	Basic();
	virtual ~Basic();
	void Initialize(const Window::Parameters& parameters);
	void Bind();
	void Unbind();
	void Draw();

private:
	Framebuffer framebuffer;
	Renderbuffer renderbuffer;
	Texture empty;
	NDCQuad quad;
	Shader shader;
};