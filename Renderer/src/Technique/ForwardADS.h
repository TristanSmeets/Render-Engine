#pragma once
#include "Technique/RenderTechnique.h"

#include "Core/Window.h"
#include "PostProcessing/PostProcessing.h"
#include "PostProcessing/Forward/Bloom.h"
#include "PostProcessing/Forward/Basic.h"
#include "Rendering/ShadowMapping.h"

class ForwardADS : public RenderTechnique
{
public:
	ForwardADS(Window& window);
	~ForwardADS();

	void Initialize(Scene& scene);
	void Render(Scene& scene);

private:
	void SetupShaders(Scene& scene);
	void SetADSLightingUniforms(Scene & scene, const std::vector<Light> & lights);

	Window& window;
	Shader lamp;
	Shader adsLighting;
	Bloom bloom;
	Basic basic;
	ShadowMapping shadowMapping;
};