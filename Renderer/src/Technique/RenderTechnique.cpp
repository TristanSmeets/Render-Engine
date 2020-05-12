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

void RenderTechnique::SetADSParameters(const ADSParameters & adsParameters)
{
	this->adsParameters = adsParameters;
}

void RenderTechnique::SetPBRParameters(const PBRParameters & pbrParameters)
{
	this->pbrParameters = pbrParameters;
}

void RenderTechnique::SetDeferredParameters(const DeferredParameters & deferredParameters)
{
	this->deferredParameters = deferredParameters;
}

const RenderTechnique::ADSParameters & RenderTechnique::GetADSParameters() const
{
	return adsParameters;
}

const RenderTechnique::PBRParameters & RenderTechnique::GetPBRParameters() const
{
	return pbrParameters;
}

const RenderTechnique::DeferredParameters & RenderTechnique::GetDeferredParameters() const
{
	return deferredParameters;
}

const PostProcessing & RenderTechnique::GetPostProcessing() const
{
	return *postProcessing;
}

RenderTechnique::ADSParameters::ADSParameters(float ambientStrength) :
	AmbientStrength(ambientStrength)
{
}

RenderTechnique::ADSParameters& RenderTechnique::ADSParameters::operator=(const ADSParameters & rhs)
{
	this->AmbientStrength = rhs.AmbientStrength;
	this->Shininess = rhs.Shininess;
	return *this;
}

RenderTechnique::PBRParameters::PBRParameters(const glm::vec3 & nonMetallicReflectionColour) :
	NonMetallicReflectionColour(nonMetallicReflectionColour)
{
}

RenderTechnique::PBRParameters & RenderTechnique::PBRParameters::operator=(const PBRParameters & rhs)
{
	this->NonMetallicReflectionColour = rhs.NonMetallicReflectionColour;
	return *this;
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
