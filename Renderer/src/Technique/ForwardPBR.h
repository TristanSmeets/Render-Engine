#pragma once
#include "Technique/RenderTechnique.h"
#include "Core/Window.h"

class ForwardPBR : public RenderTechnique
{
public:
	ForwardPBR(Window& window);
	~ForwardPBR();

	void Initialize(Scene& scene);
	void Render(Scene& scene);

private:
	Window& window;
	Shader pbr;
};
