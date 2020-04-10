#pragma once
#include "imgui.h"
#include "Core/Light.h"
#include "Core/Window.h"
#include "Core/Transform.h"

class GUIHelper
{
public:
	GUIHelper();
	virtual ~GUIHelper();
	void Initialize(const Window& window);
	void Render();

private:
	void RenderLayout();
	void RenderFPS();
	void RenderText(const char* text, ...);
	void RenderTransform(const Transform& transform);
	void RenderColour(const glm::vec3& colour);
	void RenderVec3(const char* name,const glm::vec3& vec3);
	void RenderInt(const char* name, int& value, int minimum, int maximum);

	Transform testTransform;
	Light light = Light();
	int test = 0;
};