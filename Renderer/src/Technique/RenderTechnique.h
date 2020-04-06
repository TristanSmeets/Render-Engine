#pragma once
#include "Core/Scene.h"
#include "Rendering/Shader.h"

class RenderTechnique
{
public:
	RenderTechnique();
	virtual ~RenderTechnique();
	virtual void Initialize(Scene& scene) = 0;
	virtual void Render(Scene& scene) = 0;

protected:
	Shader skyboxShader;
	Shader lamp;
};