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
	delete renderTechnique;
}

void Application::Initialize()
{
	printf("Initializing application\n");
	window.Initialize(Window::Parameters());
	InitializeGlad();
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	GLint nExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);

	printf("GL Vendor\t\t\t: %s\n", vendor);
	printf("GL Renderer\t\t\t: %s\n", renderer);
	printf("GL Version (string)\t\t: %s\n", version);
	printf("GL Version (int)\t\t: %d.%d\n", major, minor);
	printf("GLSL Version\t\t\t: %s\n", glslVersion);

	scene.Initialize();
	//renderTechnique = new ForwardADS(window);
	//renderTechnique = new ForwardPBR(window);
	//renderTechnique = new DeferredADS(window);
	renderTechnique = new DeferredPBR(window);
	renderTechnique->Initialize(scene);
	ui.Initialize(window);
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

		scene.GetCamera().Update(deltaTime);
		std::vector<Light>& lights = scene.GetLights();
		for (unsigned int i = 0; i < scene.GetNumberOfLights(); ++i)
		{
			lights[i].Update(deltaTime);
		}

		window.ProcessKeyInput();

		renderTechnique->Render(scene);

		ui.StartFrame();
		ui.Render(scene);
		ui.Render(*renderTechnique);
		ui.EndFrame();
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
