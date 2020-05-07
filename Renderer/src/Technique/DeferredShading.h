#pragma once
#include "Technique/RenderTechnique.h"
#include "Utility/Framebuffer.h"
#include "Utility/Renderbuffer.h"
#include "Rendering/Texture.h"
#include "PostProcessing/Forward/Basic.h"

class DeferredShading : public RenderTechnique
{
public:
	DeferredShading(const Window& window);
	virtual ~DeferredShading();
	void Initialize(Scene& scene);
	void Render(Scene& scene);

private:
	void SetupGBuffer(const Window::Parameters &parameters);
	void SetupSSAOBuffers(const Window::Parameters &parameters);
	void CreateSSAOKernel(std::uniform_real_distribution<GLfloat> &randomFloats, std::default_random_engine &generator);
	void CreateNoiseTexture(std::uniform_real_distribution<GLfloat> &randomFloats, std::default_random_engine &generator);
	void SetupShaders(Scene & scene);
	void GeometryPass(const glm::mat4 &view, Scene & scene);
	void SSAOTexturePass();
	void BlurPass();
	void LightingPass(const std::vector<Light> & lights, Scene & scene);
	void GBufferToDefaultFramebuffer();
	void RenderLights(const glm::mat4 &view, const std::vector<Light> & lights);
	
	const Window& window;
	//Post processing
	Basic basic;

	Framebuffer aoBuffers[2];
	Framebuffer gBuffer;
	Renderbuffer renderbuffer;
	Texture gBufferTextures[3];
	Texture aoTextures[2];
	Texture noise;
	Shader lamp;
	Shader geometryShader;
	Shader ssao;
	Shader ssaoBlur;
	Shader ssaoLighting;
	NDCQuad quad;

	glm::vec3 ssaoKernel[64];
};