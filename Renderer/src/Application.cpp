#include "Rendererpch.h"
#include "Application.h"
#include "Technique/ForwardPBR.h"

Application::Application()
{
}

Application::~Application()
{
	delete renderTechnique;
}

void Application::Initialize()
{
	window.Initialize(Window::Parameters());
	InitializeGlad();
	scene = Scene(window);
	scene.Initialize();
	renderTechnique = new ForwardPBR(window);
	renderTechnique->Initialize(scene);
}

void Application::Run()
{ 
	while (window.IsOpen())
	{
		renderTechnique->Render(scene);
		window.ProcessKeyInput();
		window.PollEvents();
		window.SwapBuffers();
	}
}

bool Application::InitializeGlad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return false;
	}
	return true;
}