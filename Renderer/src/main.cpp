#include "Rendererpch.h"

#include "Application.h"


int main()
{
	printf("Starting application\n");
	Application application;
	application.Initialize();
	application.Run();
}