#pragma once
#include "PostProcessing/PostProcessing.h"
#include "Utility/GaussianBlur.h"

class DepthOfField : public PostProcessing
{
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
	GaussianBlur gaussian;
	Texture colourBuffer;
	Texture blurredScene;
	NDCQuad quad;

};