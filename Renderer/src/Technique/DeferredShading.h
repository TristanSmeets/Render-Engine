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
	const Window& window;
	Framebuffer gBuffer;
	Texture gBufferTextures[3];
	Renderbuffer renderbuffer;
};