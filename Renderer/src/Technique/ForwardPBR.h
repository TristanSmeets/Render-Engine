#pragma once
#include "Technique/RenderTechnique.h"

#include "Core/Window.h"
#include "Utility/Cubemap.h"
#include "Utility/Framebuffer.h"
#include "PostProcessing/PostProcessing.h"

class ForwardPBR : public RenderTechnique
{
public:
	ForwardPBR(Window& window);
	~ForwardPBR();

	void Initialize(Scene& scene);
	void Render(Scene& scene);

private:
	Window& window;
	PostProcessing postProcessing;
	Shader lamp;
	Shader pbr;
	Shader directionalShadowDepth;
	Shader pointShadowDepth;
	Framebuffer directionalDepthBuffer;
	Texture shadow;
	GLuint shadowTexture;
	Cubemap ShadowCubeMap;
	Framebuffer pointDepthBuffer;

	const int shadowWidth = 1024;
	const int shadowHeight = 1024;
};
