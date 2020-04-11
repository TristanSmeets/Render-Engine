#include "Rendererpch.h"
#include "RenderTechnique.h"
#include "Utility/Filepath.h"

RenderTechnique::RenderTechnique() :
	skyboxShader(Shader(Filepath::Shader + "Background.vs", Filepath::Shader + "Background.fs"))
{
}

RenderTechnique::~RenderTechnique()
{
}
