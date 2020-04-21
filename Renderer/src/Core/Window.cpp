#include "Rendererpch.h"
#include "Window.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

Window::Window() :
	title("Default")
{
}

Window::~Window()
{
	printf("Destroying Window");
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::Initialize(const Parameters& parameters)
{
	InitializeGLFW();
	InitializeGLFWwindow(parameters);
	//glfwSwapInterval(0);
}

void Window::ProcessKeyInput()
{
	//Close window when 'esc' key gets pressed.
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void Window::PollEvents()
{
	glfwPollEvents();
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(window);
}

GLFWwindow * Window::GetGLFWwindow() const
{
	return window;
}

const Window::Parameters Window::GetWindowParameters() const
{
	Window::Parameters parameters;
	parameters.Title = title;
	glfwGetFramebufferSize(window, &parameters.Width, &parameters.Height);
	parameters.AspectRatio = (float)parameters.Width / (float)parameters.Height;
	return parameters;
}

const bool Window::IsOpen() const
{
	return !glfwWindowShouldClose(window);
}

bool Window::InitializeGLFW()
{
	printf("Initializing GLFW\n");
	if (glfwInit())
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		return true;
	}
	else
	{
		printf("Error initializing GLFW");
		return false;
	}
}

bool Window::InitializeGLFWwindow(const Parameters& parameters)
{
	printf("Initializing GLFWwindow\n");
	title = parameters.Title;
	window = glfwCreateWindow(parameters.Width, parameters.Height, parameters.Title.c_str(), nullptr, nullptr);

	if (window == nullptr)
	{
		printf("Error initializing GLFWwindow");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return false;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

Window::Parameters::Parameters(const std::string & title, int width, int height, float aspectRatio) :
	Title(title), Width(width), Height(height), AspectRatio(aspectRatio)
{
}
