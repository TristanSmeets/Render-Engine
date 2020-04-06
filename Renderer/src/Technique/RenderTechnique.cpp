#include "Rendererpch.h"
#include "RenderTechnique.h"
#include "Utility/Filepath.h"

RenderTechnique::RenderTechnique() :
	skyboxShader(Shader(Filepath::Shader + "Background.vs", Filepath::Shader + "Background.fs")),
	lamp(Shader(Filepath::Shader + "Lamp.vs", Filepath::Shader + "Lamp.fs"))
{
}

RenderTechnique::~RenderTechnique()
{
}
