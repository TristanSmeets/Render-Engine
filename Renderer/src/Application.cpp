#include "Rendererpch.h"
#include "Application.h"
#include "Technique/ForwardPBR.h"

Application::Application()
{
}

Application::~Application()
{
	printf("Destroying application\n");
	delete scene;
	delete renderTechnique;
}

void Application::Initialize()
{
	printf("Initializing application\n");
	window.Initialize(Window::Parameters());
	InitializeGlad();
	scene = new Scene(window);
	scene->Initialize();
	renderTechnique = new ForwardPBR(window);
	renderTechnique->Initialize(*scene);
	printf("Application initialization complete\n");

}

void Application::Run()
{ 
	while (window.IsOpen())
	{
		renderTechnique->Render(*scene);
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