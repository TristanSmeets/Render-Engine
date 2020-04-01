#include "Rendererpch.h"
#include "Application.h"


Application::Application()
{
}

Application::~Application()
{
}

void Application::Initialize()
{
	window.Initialize(Window::Parameters());
	InitializeGlad();
}

void Application::Run()
{ 
	while (window.IsOpen())
	{
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