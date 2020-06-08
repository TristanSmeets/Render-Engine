#pragma once

#include "PostProcessing/PostProcessing.h"
#include "PostProcessing/GaussianBlur.h"

class Bloom : public PostProcessing
{
public:
	Bloom();
	~Bloom();
	void Initialize(const Window::Parameters& parameters);
	void Bind();
	void Unbind();
	void Draw();
	void Apply();
	void Draw(const Texture& depth);
	const Framebuffer& GetFramebuffer() const;
	const Texture& GetTexture() const;
	
private:
	void BlurTextureBuffers();
	//void BlurTexture(const Texture& source, Texture& destination);
	void SetupHDRFramebuffer(const Window::Parameters & parameters);
	void SetupShaders();
	//void SetupBlurFramebuffer(const Window::Parameters & parameters);
	//float Gauss(float x, float sigma2);
	
	//Shader blur;
	Shader bloom;
	Framebuffer hdrFBO;
	Renderbuffer depthRenderbuffer;
	Texture colourBuffers[2];

	GaussianBlur gaussian;

	//Framebuffer blurFramebuffers[2];
	//Texture blurTextures[2];

	Texture bloomTexture;
	Texture blurredScene;

	NDCQuad quad;
};