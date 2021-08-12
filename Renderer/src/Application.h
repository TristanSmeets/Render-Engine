#pragma once
#include "glad/glad.h"
#include "Core/Window.h"
#include "Core/Scene.h"
#include "Technique/RenderTechnique.h"
#include "PostProcessing/PostProcessing.h"
#include "Utility/UserInterface.h"

/// <summary>
/// Creates the Window, RenderTechnique and UserInterface.
/// This class contains the main loop.
/// It creates the Window, RenderTechnique and UserInterface.
/// </summary>
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
	Scene scene;
	RenderTechnique* renderTechnique = nullptr;
	Window window = Window();
	UserInterface ui = UserInterface();

	float deltaTime = 0.0f;
	float timeLastFrame = 0.0f;
};