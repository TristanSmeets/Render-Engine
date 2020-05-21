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
	void Apply();
	const Framebuffer& GetFramebuffer() const;
	const Texture& GetTexture() const;

private:
	Framebuffer framebuffer;
	Renderbuffer renderbuffer;
	Texture empty;
	NDCQuad quad;
	Shader shader;
};