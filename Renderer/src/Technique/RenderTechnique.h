#pragma once
#include "Core/Scene.h"
#include "Rendering/Shader.h"
#include "PostProcessing/PostProcessing.h"
#include "Rendering/RenderComponent.h"

class RenderTechnique
{
public:
	struct DeferredParameters
	{
		int OcclusionPower = 2;
		int KernelSize = 64;
		float Radius = 0.5f;
		float Bias = 0.025f;

		RenderComponent::ADSParameters AdsParameters;
		RenderComponent::PBRParameters PbrParameters;

		DeferredParameters(int occlusionPower = 2, int kernelSize = 64, float radius = 0.5f, float bias = 0.025f);
		DeferredParameters& operator=(const DeferredParameters& rhs);
	};

public:
	RenderTechnique();
	virtual ~RenderTechnique();
	virtual void Initialize(Scene& scene) = 0;
	virtual void Render(Scene& scene) = 0;
	void SetDeferredParameters(const DeferredParameters& deferredParameters);
	const DeferredParameters& GetDeferredParameters() const;
	const PostProcessing& GetPostProcessing() const;

protected:
	Shader skyboxShader;

	DeferredParameters deferredParameters;
	PostProcessing* postProcessing;
};