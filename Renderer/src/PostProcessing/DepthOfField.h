#pragma once
#include "PostProcessing/PostProcessing.h"
#include "Utility/GaussianBlur.h"

class DepthOfField : public PostProcessing
{
public:
	struct Parameters
	{
		float FocalDistance = .25f;
		float FocalRange = 0.25f;
		float RangeCutoff = 0.05f;
		int Lod = 1;
	};

public:
	DepthOfField();
	~DepthOfField();
	void Initialize(const Window::Parameters& parameters);
	void Bind();
	void Unbind();
	void Draw();
	void Apply();
	void Apply(const Parameters& parameters);
	void Draw(const Texture& depthTexture);
	const Framebuffer& GetFramebuffer() const;
	const Texture& GetTexture() const;

private:
	void SetupFramebuffer(const Window::Parameters& parameters);
	void SetupShaders();

	Shader depthOfField;
	Framebuffer framebuffer;
	Renderbuffer renderbuffer;
	GaussianBlur gaussian;
	Texture colourBuffer;
	Texture blurredScene;
	NDCQuad quad;
	Parameters parameters;
};