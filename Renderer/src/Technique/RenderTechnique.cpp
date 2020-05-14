#include "Rendererpch.h"
#include "RenderTechnique.h"
#include "Utility/Filepath.h"

RenderTechnique::RenderTechnique() :
	skyboxShader(Shader(Filepath::ForwardShader + "Background.vs", Filepath::ForwardShader + "Background.fs"))
{
}

RenderTechnique::~RenderTechnique()
{
}

void RenderTechnique::SetDeferredParameters(const DeferredParameters & deferredParameters)
{
	this->deferredParameters = deferredParameters;
}

const RenderTechnique::DeferredParameters & RenderTechnique::GetDeferredParameters() const
{
	return deferredParameters;
}

const PostProcessing & RenderTechnique::GetPostProcessing() const
{
	return *postProcessing;
}

RenderTechnique::DeferredParameters::DeferredParameters(int occlusionPower, int kernelSize, float radius, float bias) :
	OcclusionPower(occlusionPower), KernelSize(kernelSize), Radius(radius), Bias(bias)
{
}

RenderTechnique::DeferredParameters & RenderTechnique::DeferredParameters::operator=(const DeferredParameters & rhs)
{
	this->OcclusionPower = rhs.OcclusionPower;
	this->KernelSize = rhs.KernelSize;
	this->Radius = rhs.Radius;
	this->Bias = rhs.Bias;
	return *this;
}
