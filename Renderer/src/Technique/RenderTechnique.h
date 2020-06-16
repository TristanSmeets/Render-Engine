#pragma once
#include "Core/Scene.h"
#include "Rendering/Shader.h"
#include "PostProcessing/PostProcessing.h"
#include "Rendering/RenderComponent.h"
#include "PostProcessing/FXAA.h"
#include "PostProcessing/DepthOfField.h"
#include "PostProcessing/Bloom.h"
#include "PostProcessing/SSAO.h"

class RenderTechnique
{
public:
	struct DeferredParameters
	{
		RenderComponent::ADSParameters AdsParameters;
		RenderComponent::PBRParameters PbrParameters;
		FXAA::Parameters FxaaParameters;
		Bloom::Parameters BloomParameters;
		DepthOfField::Parameters DofParamaters;
		SSAO::Parameters SsaoParameters;

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
	DeferredParameters& GetDeferredParameters();
	const PostProcessing& GetPostProcessing() const;

protected:
	Shader skyboxShader;

	DeferredParameters deferredParameters;
	PostProcessing* postProcessing;
};