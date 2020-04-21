#pragma once

#include "GLFW/glfw3.h"

class Window
{
public:
	struct Parameters
	{
		std::string Title;
		int Width;
		int Height;
		float AspectRatio;

		Parameters(const std::string& title = "Renderer", int width = 1280, int height = 720, float aspectRatio = (16.0f/9.0f));
	};

public:
    Window();
    ~Window();
    void Initialize(const Parameters& parameters);
    void ProcessKeyInput();
    void PollEvents();
    void SwapBuffers();
    GLFWwindow* GetGLFWwindow() const;
	const Parameters GetWindowParameters() const;
    const bool IsOpen() const;

private:
    bool InitializeGLFW();
    bool InitializeGLFWwindow(const Parameters& parameters);

    GLFWwindow* window = nullptr;
	std::string title;
};
