#pragma once
#include "imgui.h"
#include "Core/Window.h"

class GUIHelper
{
public:
	GUIHelper();
	virtual ~GUIHelper();
	void Initialize(const Window& window);
	void Render();
};