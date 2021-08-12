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

void RenderComponent::SetPBRParameters(const PBRParameters & pbrParameters)
{
	this->pbrParameters = pbrParameters;
}

const RenderComponent::PBRParameters & RenderComponent::GetPBRParameters() const
{
	return pbrParameters;
}

RenderComponent::PBRParameters & RenderComponent::GetPBRParameters()
{
	return pbrParameters;
}

RenderComponent::PBRParameters::PBRParameters(const glm::vec3 & nonMetallicReflectionColour) :
	NonMetallicReflectionColour(nonMetallicReflectionColour)
{
}

RenderComponent::PBRParameters & RenderComponent::PBRParameters::operator=(const PBRParameters & rhs)
{
	this->NonMetallicReflectionColour = rhs.NonMetallicReflectionColour;
	this->IsTransparent = rhs.IsTransparent;
	this->Roughness = rhs.Roughness;
	this->UsingSmoothness = rhs.UsingSmoothness;
	return *this;
}
