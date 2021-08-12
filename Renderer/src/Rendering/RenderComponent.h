#pragma once

#include "Rendering/Mesh.h"
#include "Rendering/Material.h"

class RenderComponent
{
public:
	struct PBRParameters
	{
		glm::vec3 NonMetallicReflectionColour;
		bool IsTransparent = false;
		float Roughness = 0.0f;
		bool UsingSmoothness = false;

		PBRParameters(const glm::vec3& nonMetallicReflectionColour = glm::vec3(0.04f));
		PBRParameters& operator=(const PBRParameters& rhs);
	};
public:
	RenderComponent();
	~RenderComponent();
	void SetMesh(const Mesh& mesh);
	void SetMaterial(const Material& material);
	const Mesh& GetMesh() const;
	const Material& GetMaterial() const;
	void SetPBRParameters(const PBRParameters& pbrParameters);
	const PBRParameters& GetPBRParameters() const;
	PBRParameters& GetPBRParameters();

private:
	const Mesh* mesh = nullptr;
	const Material* material = nullptr;
	PBRParameters pbrParameters;
};