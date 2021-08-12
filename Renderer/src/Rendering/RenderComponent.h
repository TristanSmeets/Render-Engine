#pragma once

#include "Rendering/Mesh.h"
#include "Rendering/Material.h"

class RenderComponent
{
public:
	RenderComponent();
	~RenderComponent();
	void SetMesh(const Mesh& mesh);
	void SetMaterial(const Material& material);
	void BindMaterial() const;
	const Mesh& GetMesh() const;
	const Material& GetMaterial() const;

private:
	const Mesh* mesh = nullptr;
	const Material* material = nullptr;
};