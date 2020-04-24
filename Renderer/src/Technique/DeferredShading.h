#pragma once
#include "Technique/RenderTechnique.h"
#include "Utility/Framebuffer.h"
#include "Utility/Renderbuffer.h"
#include "Rendering/Texture.h"

class DeferredShading : public RenderTechnique
{
public:
	DeferredShading(const Window& window);
	virtual ~DeferredShading();
	void Initialize(Scene& scene);
	void Render(Scene& scene);

private:
	const float attenuationConstant = 0.09f;
	const float attenuationLinear = 0.032f;
	const float attenuationQuadratic = 0.44f;
	const Window& window;

	Framebuffer gBuffer;
	Texture gBufferTextures[3];
	Renderbuffer renderbuffer;
	Shader lamp;
	Shader geometryShader;
	Shader lightingShader;
	NDCQuad quad;

	//SSAO
	Framebuffer aoColourBuffer;
	Framebuffer aoBlurBuffer;
	Texture aoColour;
	Texture aoBlur;
	Texture aoNoise;
	Shader ssaoGeometry;
	Shader ssaoLighting;
	Shader ssao;
	Shader ssaoBlur;
	glm::vec3 ssaoKernel[64];
};