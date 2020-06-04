#pragma once

#include "PostProcessing/PostProcessing.h"

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
	void SetupHDRFramebuffer(const Window::Parameters & parameters);
	void SetupShaders();
	void SetupBlurFramebuffer(const Window::Parameters & parameters);
	void CreateSampleKernel();

	
	Shader blur;
	Shader bloom;
	Framebuffer hdrFBO;
	Renderbuffer depthRenderbuffer;
	Texture colourBuffers[2];

	Framebuffer blurFramebuffers[2];
	Texture blurTextures[2];

	static const int NumberOfTaps = 8;
	glm::vec2 sampleKernel[NumberOfTaps];

	NDCQuad quad;
};