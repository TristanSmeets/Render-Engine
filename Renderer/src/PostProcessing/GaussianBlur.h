#pragma once

#include "Rendering/Shader.h"
#include "Utility/Framebuffer.h"
#include "Utility/NDCQuad.h"

class GaussianBlur
{
public:
	GaussianBlur();
	~GaussianBlur();
	void BlurTexture(const Texture& source, Texture& destination, unsigned int maxLOD);
	void SetupFramebuffers(const glm::ivec2& resolution);

private:
	void SetupShader();

	Shader blur;
	Framebuffer framebuffers[2];
	Texture textures[2];
	NDCQuad quad;
};