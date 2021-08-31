#pragma once

#include "PostProcessing/PostProcessing.h"

class Basic : public PostProcessing
{
public:
	struct Parameters
	{
		float GammaCorrection = 2.2f;
		float Exposure = 1.0f;
	};
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
	GLSLProgram shader;
	Parameters parameters;

};