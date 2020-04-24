#pragma once
#include "Technique/RenderTechnique.h"

#include "Core/Window.h"
#include "Utility/Cubemap.h"
#include "Utility/Framebuffer.h"
#include "PostProcessing/PostProcessing.h"
#include "PostProcessing/Forward/Bloom.h"
#include "PostProcessing/Forward/Basic.h"

class ForwardPBR : public RenderTechnique
{
public:
	ForwardPBR(Window& window);
	~ForwardPBR();

	void Initialize(Scene& scene);
	void Render(Scene& scene);

private:
	const static int maximumLights = 4;
	const int shadowWidth = 1024;
	const int shadowHeight = 1024;
	Window& window;
	PostProcessing* postProcessing;
	Shader lamp;
	Shader pbr;
	Shader directionalShadowDepth;
	Shader pointShadowDepth;
	Framebuffer directionalDepthBuffer;
	Texture shadow;
	GLuint shadowTexture;
	Cubemap shadowCubeMaps[maximumLights];
	Framebuffer pointDepthBuffer;

	Bloom bloom;
	Basic basic;

	float aspect = (float)shadowHeight / (float)shadowHeight;
	float nearPlane = 1.0f;
	float farPlane = 25.0f;
	glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), aspect, nearPlane, farPlane);
};
