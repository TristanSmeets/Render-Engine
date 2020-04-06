#pragma once
#include "Core/Scene.h"

__interface IRenderTechnique
{
public:
	void Render(Scene& scene) = 0;
};