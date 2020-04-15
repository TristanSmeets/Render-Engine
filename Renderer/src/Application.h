#pragma once
#include "glad/glad.h"
#include "Core/Window.h"
#include "Core/Scene.h"
#include "Technique/RenderTechnique.h"
#include "PostProcessing/PostProcessing.h"
#include "Utility/GUIHelper.h"

class Application
{
public:
	Application();
	~Application();
	void Initialize();
	void Run();

private:
	bool InitializeGlad();
	void DispatchEvents();
	Scene* scene = nullptr;
	RenderTechnique* renderTechnique = nullptr;
	Window window = Window();
	GUIHelper guiHelper = GUIHelper();

	float deltaTime = 0.0f;
	float timeLastFrame = 0.0f;
};