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
	const Window& window;
};
