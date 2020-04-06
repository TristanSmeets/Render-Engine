#pragma once
#include "glad/glad.h"
#include "Core/Window.h"
#include "Technique/RenderTechnique.h"

class Application
{
public:
	Application();
	~Application();
	void Initialize();
	void Run();

private:
	bool InitializeGlad();

	Window window = Window();
	Scene* scene = nullptr;
	RenderTechnique* renderTechnique = nullptr;
	//TODO: Add scene and rendering technique here.
};