#pragma once
#include "Utility/Framebuffer.h"
#include "Rendering/Shader.h"
#include "Utility/NDCQuad.h"

class FXAA
{
public:
	struct Parameters
	{
		glm::ivec2 Resolution = glm::ivec2(1280, 720);
		float SpanMax = 8.0f;
		float ReduceMinumum = (1.0f/128.0f);
		float ReduceMultiplier = (1.0f/8.0f);
	};
public:
	FXAA();
	~FXAA();
	void Initialize(const Parameters& parameters);
	void Bind();
	void Unbind();
	void Blit(const Framebuffer::BlitParameters& parameters);
	void Apply(const Parameters& parameters, const Texture& screenTexture);
	const Framebuffer& GetFramebuffer() const;

private:
	Framebuffer framebuffer;
	Renderbuffer renderbuffer;
	Texture fxaaColourAttachment;
	NDCQuad quad;
	Shader shader;
	Parameters fxaaParameters;
};