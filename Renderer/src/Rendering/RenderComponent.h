#pragma once

#include "Rendering/Mesh.h"
#include "Rendering/Material.h"

class RenderComponent
{
public:
	struct ADSParameters
	{
		float AmbientStrength;
		
		ADSParameters(float ambientStrength = 0.01f);
		ADSParameters& operator=(const ADSParameters& rhs);
	};
	struct PBRParameters
	{
		glm::vec3 NonMetallicReflectionColour;

		PBRParameters(const glm::vec3& nonMetallicReflectionColour = glm::vec3(0.04f));
		PBRParameters& operator=(const PBRParameters& rhs);
	};
public:
	RenderComponent();
	~RenderComponent();
	void SetMesh(const Mesh& mesh);
	void SetMaterial(const Material& material);
	void SetADSParameters(const ADSParameters& adsParameters);
	void SetPBRParameters(const PBRParameters& pbrParameters);
	const Mesh& GetMesh() const;
	const Material& GetMaterial() const;
	const ADSParameters& GetADSParameters() const;
	const PBRParameters& GetPBRParameters() const;

private:
	const Mesh* mesh = nullptr;
	const Material* material = nullptr;
	ADSParameters adsParameters;
	PBRParameters pbrParameters;
};