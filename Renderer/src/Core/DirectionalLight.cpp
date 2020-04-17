#include "Rendererpch.h"
#include "DirectionalLight.h"
#include <gtx/rotate_vector.hpp>

DirectionalLight::DirectionalLight() :
	Light("Directional", glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1.0f))
{
}

DirectionalLight::~DirectionalLight()
{
	Light::~Light();
}

const glm::mat4 DirectionalLight::GetView() const
{
	glm::vec3 position = GetWorldPosition();
	return glm::lookAt(position, position + GetFront(), GetUp());
}

const glm::vec3 DirectionalLight::GetFront() const
{
	glm::vec3 eulerAngles = transform.GetRotation();
	glm::vec3 direction;
	direction.x = cos(glm::radians(eulerAngles.y)) * cos(glm::radians(eulerAngles.x));
	direction.y = sin(glm::radians(eulerAngles.x));
	direction.z = sin(glm::radians(eulerAngles.y)) * cos(glm::radians(eulerAngles.x));
	return glm::normalize(direction);
}

const glm::vec3 DirectionalLight::GetSide() const
{
	return  glm::normalize(glm::cross(GetFront(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

const glm::vec3 DirectionalLight::GetUp() const
{
	return glm::normalize(glm::cross(GetSide(), GetFront()));
}
