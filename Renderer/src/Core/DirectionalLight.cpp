#include "Rendererpch.h"
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() :
	Light("Directional", glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1.0f))
{
}

DirectionalLight::~DirectionalLight()
{
	Light::~Light();
}

const glm::vec3 DirectionalLight::GetDirection() const
{
	glm::vec3 eulerAngles = transform.GetRotation();
	glm::vec3 direction;
	direction.x = cos(glm::radians(eulerAngles.y)) * cos(glm::radians(eulerAngles.x));
	direction.y = sin(glm::radians(eulerAngles.x));
	direction.z = sin(glm::radians(eulerAngles.y)) * cos(glm::radians(eulerAngles.x));
	return glm::normalize(direction);
}
