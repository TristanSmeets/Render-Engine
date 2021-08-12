#pragma once

#include "GLFW/glfw3.h"

/// <summary>
/// Class that is responsible for all things window related.
/// </summary>
class Window
{
public:
	/// <summary>
	/// Initialization parameters.
	/// </summary>
	struct Parameters
	{
		std::string Title;	///< Title of the window
		int Width;			///< Width of the window
		int Height;			///< Height of the window
		float AspectRatio;	///< Aspect ratio of the window

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
