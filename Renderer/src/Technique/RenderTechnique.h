#pragma once
#include "Core/Scene.h"
#include "Rendering/Shader.h"

class RenderTechnique
{
public:
	struct ADSParameters
	{
		float AmbientStrength;

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

		DeferredParameters(int occlusionPower = 2);
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
protected:
	Shader skyboxShader;
	ADSParameters adsParameters;
	PBRParameters pbrParameters;
	DeferredParameters deferredParameters;
};