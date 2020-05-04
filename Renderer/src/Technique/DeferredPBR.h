#pragma once
#include "Technique/RenderTechnique.h"
#include "Utility/Framebuffer.h"
#include "Utility/Renderbuffer.h"
#include "Rendering/Texture.h"

class DeferredPBR : public RenderTechnique
{
public:
	DeferredPBR(const Window& window);
	virtual ~DeferredPBR();
	void Initialize(Scene& scene);
	void Render(Scene& scene);

private:
	void SetupGBuffers(const Window::Parameters& parameters);
	void SetupShaders(Scene& scene);
	void GeometryPass(const glm::mat4 &view, Scene & scene);
	void LightingPass(const std::vector<Light> & lights, Scene & scene);
	void GBufferToDefaultFramebuffer();
	void RenderLights(const glm::mat4 &view, const std::vector<Light> & lights);
	const Window& window;

	Framebuffer gBuffer;
	Renderbuffer renderbuffer;
	Texture gBufferTextures[4];
	Shader lamp;
	Shader geometry;
	Shader pbrLighting;
	NDCQuad quad;
};
