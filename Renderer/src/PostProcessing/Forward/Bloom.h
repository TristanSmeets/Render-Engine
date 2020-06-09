#pragma once

#include "PostProcessing/PostProcessing.h"
#include "Utility/GaussianBlur.h"

class Bloom : public PostProcessing
{
public:
	struct Parameters
	{
		int Lod = 1;
		float Exposure = 1.0f;
		float GammaCorrection = 2.2f;
	};
public:
	Bloom();
	~Bloom();
	void Initialize(const Window::Parameters& parameters);
	void Bind();
	void Unbind();
	void Draw();
	void Apply();
	const Framebuffer& GetFramebuffer() const;
	const Texture& GetTexture() const;
	
private:
	void BlurTextureBuffers();
	void SetupHDRFramebuffer(const Window::Parameters & parameters);
	void SetupShaders();
	
	Shader bloom;
	Framebuffer hdrFBO;
	Renderbuffer depthRenderbuffer;
	Texture colourBuffers[2];
	GaussianBlur gaussian;
	Texture bloomTexture;	
	Parameters parameters;


	NDCQuad quad;
};