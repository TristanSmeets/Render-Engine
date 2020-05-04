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

const RenderTechnique::ADSParameters & RenderTechnique::GetADSParameters() const
{
	return adsParameters;
}

const RenderTechnique::PBRParameters & RenderTechnique::GetPBRParameters() const
{
	return pbrParameters;
}

RenderTechnique::ADSParameters::ADSParameters(float ambientStrength) :
	AmbientStrength(ambientStrength)
{
}

RenderTechnique::ADSParameters& RenderTechnique::ADSParameters::operator=(const ADSParameters & rhs)
{
	this->AmbientStrength = rhs.AmbientStrength;
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
