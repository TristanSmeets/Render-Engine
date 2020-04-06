#pragma once
#include "glad/glad.h"
#include "Core/Window.h"
#include "Core/Scene.h"
#include "Technique/RenderTechnique.h"
#include "Technique/ForwardPBR.h"

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
	Scene scene;
	RenderTechnique* renderTechnique = nullptr;
};