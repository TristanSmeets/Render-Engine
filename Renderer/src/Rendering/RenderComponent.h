#pragma once

#include "Rendering/Mesh.h"
#include "Rendering/Material.h"

class RenderComponent
{
public:
	struct ADSParameters
	{
		float AmbientStrength;
		float Shininess;
		bool IsTransparent = false;

		ADSParameters(float ambientStrength = 0.01f, float shininess = 256.0f);
		ADSParameters& operator=(const ADSParameters& rhs);
	};
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
	void SetADSParameters(const ADSParameters& adsParameters);
	void SetPBRParameters(const PBRParameters& pbrParameters);
	const ADSParameters& GetADSParameters() const;
	const PBRParameters& GetPBRParameters() const;
	ADSParameters& GetADSParameters();
	PBRParameters& GetPBRParameters();

private:
	const Mesh* mesh = nullptr;
	const Material* material = nullptr;
	ADSParameters adsParameters;
	PBRParameters pbrParameters;
};