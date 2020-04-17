#pragma once
#include "Core/Light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	virtual ~DirectionalLight();
	const glm::vec3 GetDirection() const;
};