#include "Rendererpch.h"
#include "RenderComponent.h"

RenderComponent::RenderComponent() 
{
}

RenderComponent::~RenderComponent()
{
}

void RenderComponent::SetMesh(const Mesh & mesh)
{
	this->mesh = &mesh;
}

void RenderComponent::SetMaterial(const Material & material)
{
	this->material = &material;
}

const Mesh & RenderComponent::GetMesh() const
{
	return *mesh;
}

const Material & RenderComponent::GetMaterial() const
{
	return *material;
}

void RenderComponent::SetADSParameters(const ADSParameters & adsParameters)
{
	this->adsParameters = adsParameters;
}

void RenderComponent::SetPBRParameters(const PBRParameters & pbrParameters)
{
	this->pbrParameters = pbrParameters;
}

const RenderComponent::ADSParameters & RenderComponent::GetADSParameters() const
{
	return adsParameters;
}

const RenderComponent::PBRParameters & RenderComponent::GetPBRParameters() const
{
	return pbrParameters;
}

RenderComponent::ADSParameters & RenderComponent::GetADSParameters()
{
	return adsParameters;
}

RenderComponent::PBRParameters & RenderComponent::GetPBRParameters()
{
	return pbrParameters;
}

RenderComponent::ADSParameters::ADSParameters(float ambientStrength, float shininess) :
	AmbientStrength(ambientStrength), Shininess(shininess)
{
}

RenderComponent::ADSParameters & RenderComponent::ADSParameters::operator=(const ADSParameters & rhs)
{	
	this->AmbientStrength = rhs.AmbientStrength;
	this->Shininess = rhs.Shininess;
	this->IsTransparent = rhs.IsTransparent;
	return *this;
}

RenderComponent::PBRParameters::PBRParameters(const glm::vec3 & nonMetallicReflectionColour) :
	NonMetallicReflectionColour(nonMetallicReflectionColour)
{
}

RenderComponent::PBRParameters & RenderComponent::PBRParameters::operator=(const PBRParameters & rhs)
{
	this->NonMetallicReflectionColour = rhs.NonMetallicReflectionColour;
	this->IsTransparent = rhs.IsTransparent;
	return *this;
}
