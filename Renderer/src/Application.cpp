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
	delete postProcessing;
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
	postProcessing = new PostProcessing();
	postProcessing->Initialize(Window::Parameters());
	guiHelper.Initialize(window);
	printf("Application initialization complete\n");

}

void Application::Run()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (window.IsOpen())
	{
		float timeCurrentFrame = (float)glfwGetTime();
		deltaTime = timeCurrentFrame - timeLastFrame;
		timeLastFrame = timeCurrentFrame;

		scene->GetCamera().Update(deltaTime);

		window.ProcessKeyInput();

		postProcessing->Bind();
		renderTechnique->Render(*scene);
		postProcessing->Unbind();
		postProcessing->Draw();
		
		guiHelper.Render(*scene);

		DispatchEvents();
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

void Application::DispatchEvents()
{
	//TODO: Call dispatch on all eventqueues in here.
}
