#include "Rendererpch.h"
#include "Application.h"
#include "Rendering/GLSLProgramException.h"

int main()
{
	printf("Starting application\n");
	Application application;
	try
	{
		application.Initialize();
	}
	catch (GLSLProgramException& e)
	{
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	application.Run();
}
