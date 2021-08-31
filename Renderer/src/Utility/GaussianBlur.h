#pragma once

#include "Rendering/GLSLProgram.h"
#include "Utility/Framebuffer.h"
#include "Utility/NDCQuad.h"

class GaussianBlur
{
public:
	struct Parameters
	{
		unsigned int AmountOfWeightsToUse = 5;
		float Sigma2 = 4.0f;
	};
public:
	GaussianBlur();
	~GaussianBlur();
	void BlurTexture(const Texture& source, Texture& destination, const Parameters& parameters);
	void SetupFramebuffers(const glm::ivec2& resolution);

private:
	void SetupShader();
	float Gauss(float x, float sigma2);

	GLSLProgram blur;
	Framebuffer framebuffers[2];
	Texture textures[2];
	NDCQuad quad;
};