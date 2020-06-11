#pragma once

#include "Utility/Framebuffer.h"
#include "Utility/Cubemap.h"
#include "Core/Light.h"
#include "Core/Actor.h"
#include "Rendering/Shader.h"

class ShadowMapping
{
public:
	struct Parameters
	{
		glm::ivec2 Resolution = glm::ivec2(1024, 1024);
		float AspectRatio = (float)Resolution.x / (float)Resolution.y;
		float NearPlane = 1.0f;
		float FarPlane = 25.0f;
	};
public:
	ShadowMapping();
	virtual ~ShadowMapping();
	void Initialize(const Parameters& parameters);
	void MapPointLights(const std::vector<Light>& lights, const std::vector<Actor>& actors, int lightsToRender = 3);
	void SetParameters(const Parameters& parameters);
	const Parameters& GetParameters() const;
	const int GetMaximumNumberOfLights() const;
	void BindShadowMap(int index);
	

private:
	void SetupPointLightBuffer();
	
	const static int MaximumNumberOfLights = 10;
	Cubemap cubeMaps[MaximumNumberOfLights];
	ShadowMapping::Parameters parameters;
	Framebuffer depthBuffer;
	Shader shadowDepth;
};