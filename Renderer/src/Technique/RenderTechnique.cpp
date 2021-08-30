#include "Rendererpch.h"
#include "RenderTechnique.h"
#include "Utility/Filepath.h"

RenderTechnique::RenderTechnique()
{
	skyboxShader.CompileShader(Filepath::ForwardShader + "Background.vs");
	skyboxShader.CompileShader(Filepath::ForwardShader + "Background.fs");
	skyboxShader.Link();
	skyboxShader.Validate();
}

RenderTechnique::~RenderTechnique()
{
}

void RenderTechnique::SetDeferredParameters(const DeferredParameters& deferredParameters)
{
	this->deferredParameters = deferredParameters;
}

const RenderTechnique::DeferredParameters& RenderTechnique::GetDeferredParameters() const
{
	return deferredParameters;
}

RenderTechnique::DeferredParameters& RenderTechnique::GetDeferredParameters()
{
	return deferredParameters;
}

const PostProcessing& RenderTechnique::GetPostProcessing() const
{
	return *postProcessing;
}

RenderTechnique::DeferredParameters::DeferredParameters()
{
}

RenderTechnique::DeferredParameters& RenderTechnique::DeferredParameters::operator=(const DeferredParameters& rhs)
{
	return *this;
}
