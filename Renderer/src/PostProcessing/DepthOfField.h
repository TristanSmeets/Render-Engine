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
	virtual void Initialize(const Window::Parameters& parameters);
	virtual void Bind();
	virtual void Unbind();
	virtual void Draw();
	virtual void Apply();
	virtual void Draw(const Texture& depthTexture);
	virtual const Framebuffer& GetFramebuffer() const;
	virtual const Texture& GetTexture() const;

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