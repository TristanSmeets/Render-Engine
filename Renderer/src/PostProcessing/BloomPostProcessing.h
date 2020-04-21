#pragma once

#include "Utility/Framebuffer.h"
#include "Utility/Renderbuffer.h"
#include "Utility/NDCQuad.h"
#include "Core/Window.h"
#include "Rendering/Texture.h"

class BloomPostProcessing
{
public:
	BloomPostProcessing();
	~BloomPostProcessing();
	void Initialize(const Window::Parameters& parameters);
	void BindHDR();
	void Unbind();
	void Draw();
	
private:
	void BlurTextureBuffers();
	
	Shader blur;
	Shader bloom;
	Framebuffer hdrFBO;
	Renderbuffer depthRenderbuffer;
	Texture colourBuffers[2];

	Framebuffer pingpongFramebuffers[2];
	Texture pingpongTextures[2];

	NDCQuad quad;
};