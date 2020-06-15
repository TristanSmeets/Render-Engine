#include "Rendererpch.h"
#include "Application.h"
#include "Technique/ForwardPBR.h"
#include "Technique/DeferredADS.h"
#include "Technique/ForwardADS.h"
#include "Technique/DeferredPBR.h"

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
	//renderTechnique = new ForwardADS(window);
	//renderTechnique = new ForwardPBR(window);
	//renderTechnique = new DeferredADS(window);
	renderTechnique = new DeferredPBR(window);
	renderTechnique->Initialize(*scene);
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

		renderTechnique->Render(*scene);

		guiHelper.StartFrame();
		guiHelper.Render(*scene);
		guiHelper.Render(*renderTechnique);
		guiHelper.EndFrame();
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
	EventQueue<KeyEvent>::Dispatch();
}
