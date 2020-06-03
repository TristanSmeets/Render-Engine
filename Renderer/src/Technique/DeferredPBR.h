#pragma once
#include "Technique/RenderTechnique.h"
#include "Utility/Framebuffer.h"
#include "Utility/Renderbuffer.h"
#include "Rendering/Texture.h"
#include "PostProcessing/Forward/Basic.h"
#include "Rendering/ShadowMapping.h"
#include "Utility/FXAA.h"
#include "PostProcessing/Forward/Bloom.h"

class DeferredPBR : public RenderTechnique
{
public:
	DeferredPBR(const Window& window);
	virtual ~DeferredPBR();
	void Initialize(Scene& scene);
	void Render(Scene& scene);

private:
	void SetupGBuffers(const Window::Parameters& parameters);
	void SetupSSAOBuffers(const Window::Parameters& parameters);
	void CreateSSAOKernel(std::uniform_real_distribution<GLfloat>& randomFloats, std::default_random_engine& generator);
	void CreateNoiseTexture(std::uniform_real_distribution<GLfloat>& randomFloats, std::default_random_engine& generator);
	void SetupShaders(Scene& scene);
	void GeometryPass(const glm::mat4 &view, Scene & scene);
	void SSAOTexturePass();
	void BlurPass();
	void LightingPass(const std::vector<Light> & lights, Scene & scene);
	void RenderLights(const glm::mat4 &view, const std::vector<Light> & lights);
	void RenderTransparentActors(Scene& scene);
	void SetPBRShaderUniforms(const Camera& camera, const Skybox& skybox, const std::vector<Light>& lights);

	const Window& window;
	Basic basic;
	Bloom bloom;

	Framebuffer gBuffer;
	Framebuffer aoBuffers[2];
	Renderbuffer renderbuffer;
	Texture gBufferTextures[5];
	Texture aoTextures[2];
	Texture noise;
	Shader lamp;
	Shader geometry;
	Shader pbrLighting;
	Shader ssao;
	Shader ssaoBlur;
	NDCQuad quad;
	ShadowMapping shadowMapping;
	Shader forwardLighting;
	FXAA fxaa;

	glm::vec3 ssaoKernel[64];

};
