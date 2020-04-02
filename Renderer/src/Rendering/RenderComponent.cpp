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

const Mesh & RenderComponent::GetMesh()
{
	return *mesh;
}

const Material & RenderComponent::GetMaterial()
{
	return *material;
}
