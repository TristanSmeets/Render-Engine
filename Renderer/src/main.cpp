#include "Rendererpch.h"

#include "Core/Actor.h"
#include "Core/Light.h"
#include "Core/Window.h"


int main()
{
	Window window = Window();
	Window::Parameters windowParameters = Window::Parameters("Beach Renderer", 1280, 720, (16.0f / 9.0f));
	window.Initialize(windowParameters);

	while (window.IsOpen())
	{
		window.ProcessKeyInput();
		window.PollEvents();
		window.SwapBuffers();
	}
}