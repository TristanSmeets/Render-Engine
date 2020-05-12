#pragma once
#include "Core/Scene.h"
#include "Rendering/Shader.h"
#include "PostProcessing/PostProcessing.h"

class RenderTechnique
{
public:
	struct ADSParameters
	{
		float AmbientStrength;
		float Shininess = 256.0f;

		ADSParameters(float ambientStrength = 0.01f);
		ADSParameters& operator=(const ADSParameters& rhs);
	};
	struct PBRParameters
	{
		glm::vec3 NonMetallicReflectionColour;

		PBRParameters(const glm::vec3& nonMetallicReflectionColour = glm::vec3(0.04f));
		PBRParameters& operator=(const PBRParameters& rhs);
	};
	struct DeferredParameters
	{
		int OcclusionPower = 2;
		int KernelSize = 64;
		float Radius = 0.5f;
		float Bias = 0.025f;

		DeferredParameters(int occlusionPower = 2, int kernelSize = 64, float radius = 0.5f, float bias = 0.025f);
		DeferredParameters& operator=(const DeferredParameters& rhs);
	};

public:
	RenderTechnique();
	virtual ~RenderTechnique();
	virtual void Initialize(Scene& scene) = 0;
	virtual void Render(Scene& scene) = 0;
	void SetADSParameters(const ADSParameters& adsParameters);
	void SetPBRParameters(const PBRParameters& pbrParameters);
	void SetDeferredParameters(const DeferredParameters& deferredParameters);
	const ADSParameters& GetADSParameters() const;
	const PBRParameters& GetPBRParameters() const;
	const DeferredParameters& GetDeferredParameters() const;
	const PostProcessing& GetPostProcessing() const;

protected:
	Shader skyboxShader;
	ADSParameters adsParameters;
	PBRParameters pbrParameters;
	DeferredParameters deferredParameters;
	PostProcessing* postProcessing;
};