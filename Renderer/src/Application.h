#pragma once
#include "glad/glad.h"
#include "Core/Window.h"

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
	//TODO: Add scene and rendering technique here.
};