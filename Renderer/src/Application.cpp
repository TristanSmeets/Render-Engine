#include "Rendererpch.h"
#include "Application.h"
#include "Technique/DeferredPBR.h"
#include "Utility/Log.h"

Application::Application()
{
}

Application::~Application()
{
	delete renderTechnique;
}

void Application::Initialize()
{
	Log::Init();
	Log::Trace("Initializing application.");
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

	Log::Trace("GL Vendor\t\t\t: {}", vendor);
	Log::Trace("GL Renderer\t\t: {}", renderer);
	Log::Trace("GL Version (string)\t: {}", version);
	Log::Trace("GL Version (int)\t\t: {}.{}", major, minor);
	Log::Trace("GLSL Version\t\t: {}", glslVersion);

	scene.Initialize();
	renderTechnique = new DeferredPBR(window);
	renderTechnique->Initialize(scene);
	ui.Initialize(window);
	Log::Trace("Application initialization complete");
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
	Log::Close();
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
