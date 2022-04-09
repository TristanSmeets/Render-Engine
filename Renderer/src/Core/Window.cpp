#include "Rendererpch.h"
#include "Window.h"
#include "Event/EventQueue.h"
#include "Event/KeyEvent.h"
#include <Utility/Log.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

Window::Window() :
	title("Default")
{
}

Window::~Window()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::Initialize(const Parameters& parameters)
{
	InitializeGLFW();
	InitializeGLFWwindow(parameters);
	glfwSwapInterval(1);
}

void Window::ProcessKeyInput()
{
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		EventQueue<KeyEvent>::QueueEvent(KeyEvent(KeyEvent::A, KeyEvent::PRESS));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		EventQueue<KeyEvent>::QueueEvent(KeyEvent(KeyEvent::S, KeyEvent::PRESS));
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		EventQueue<KeyEvent>::QueueEvent(KeyEvent(KeyEvent::W, KeyEvent::PRESS));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		EventQueue<KeyEvent>::QueueEvent(KeyEvent(KeyEvent::D, KeyEvent::PRESS));
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		EventQueue<KeyEvent>::QueueEvent(KeyEvent(KeyEvent::Q, KeyEvent::PRESS));
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		EventQueue<KeyEvent>::QueueEvent(KeyEvent(KeyEvent::E, KeyEvent::PRESS));
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		EventQueue<KeyEvent>::QueueEvent(KeyEvent(KeyEvent::R, KeyEvent::PRESS));
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		EventQueue<KeyEvent>::QueueEvent(KeyEvent(KeyEvent::F, KeyEvent::PRESS));
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		EventQueue<KeyEvent>::QueueEvent(KeyEvent(KeyEvent::T, KeyEvent::PRESS));
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		EventQueue<KeyEvent>::QueueEvent(KeyEvent(KeyEvent::G, KeyEvent::PRESS));
	}
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
	Log::Trace("Initializing GLFW");
	if (glfwInit())
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		return true;
	}
	else
	{
		Log::Error("Error initializing GLFW");
		return false;
	}
}

bool Window::InitializeGLFWwindow(const Parameters& parameters)
{
	Log::Trace("Initializing GLFWwindow");
	title = parameters.Title;
	window = glfwCreateWindow(parameters.Width, parameters.Height, parameters.Title.c_str(), nullptr, nullptr);

	if (window == nullptr)
	{
		Log::Error("Error initializing GLFWwindow");
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
