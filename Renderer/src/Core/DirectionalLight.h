#pragma once
#include "Core/Light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	virtual ~DirectionalLight();
	const glm::mat4 GetView() const;
	const glm::vec3 GetFront() const;
	const glm::vec3 GetSide() const;
	const glm::vec3 GetUp() const;
};