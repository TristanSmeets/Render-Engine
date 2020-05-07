#pragma once
#include "Technique/RenderTechnique.h"

#include "Core/Window.h"
#include "PostProcessing/PostProcessing.h"
#include "PostProcessing/Forward/Bloom.h"
#include "PostProcessing/Forward/Basic.h"

class ForwardADS : public RenderTechnique
{
public:
	ForwardADS(Window& window);
	~ForwardADS();

	void Initialize(Scene& scene);
	void Render(Scene& scene);

private:
	void SetupShaders(Scene& scene);
	void SetupPointLightBuffer();
	void CreatePointLightShadows(const std::vector<Light> & lights, const std::vector<Actor> & actors);
	void SetADSLightingUniforms(Scene & scene, const std::vector<Light> & lights);

	const static int maximumLights = 10;
	const int shadowWidth = 1024;
	const int shadowHeight = 1024;

	Window& window;
	Shader lamp;
	Shader adsLighting;
	Shader pointShadowDepth;
	Cubemap shadowCubeMaps[maximumLights];
	Framebuffer pointDepthBuffer;
	Bloom bloom;
	Basic basic;

	float aspect = (float)shadowWidth / (float)shadowHeight;
	float nearPlane = 1.0f;
	float farPlane = 25.0f;
	glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), aspect, nearPlane, farPlane);
};