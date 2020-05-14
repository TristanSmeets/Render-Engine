#pragma once
#include "Technique/RenderTechnique.h"

#include "Core/Window.h"
#include "Utility/Cubemap.h"
#include "Utility/Framebuffer.h"
#include "PostProcessing/PostProcessing.h"
#include "PostProcessing/Forward/Bloom.h"
#include "PostProcessing/Forward/Basic.h"
#include "Rendering/ShadowMapping.h"
#include "Utility/MSAA.h"

class ForwardPBR : public RenderTechnique
{
public:
	ForwardPBR(Window& window);
	~ForwardPBR();

	void Initialize(Scene& scene);
	void Render(Scene& scene);

private:
	void SetupShaders(Scene & scene);
	void SetPBRShaderUniforms(Scene & scene, const Skybox & skybox, const std::vector<Light> & lights);

	Window& window;
	Shader lamp;
	Shader pbr;
	ShadowMapping shadowMapping;
	MSAA msaa;

	Bloom bloom;
	Basic basic;
};
