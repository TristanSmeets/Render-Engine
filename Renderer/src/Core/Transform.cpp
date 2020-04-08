#include "Rendererpch.h"
#include "Transform.h"

#include <gtx/transform.hpp>

Transform::Transform(const glm::vec3 & position, const glm::vec3 & rotation, const glm::vec3 & scale)
{
	Rotate(rotation);
	Translate(position);
	Scale(scale);
}

Transform::Transform(const Transform & rhs) :
	position(rhs.position), rotation(rhs.rotation), scale(rhs.scale)
{
}

Transform::~Transform()
{
}

void Transform::Translate(const glm::vec3 & translation)
{
	position += translation;
}

void Transform::Rotate(const glm::vec3 & eulerAngles)
{
	rotation += eulerAngles;
}

void Transform::Scale(const glm::vec3 & scale)
{
	this->scale = scale;
}

const glm::vec3 Transform::GetPosition() const
{
	return position;
}

const glm::vec3 Transform::GetRotation() const
{
	return rotation;
}

const glm::vec3 Transform::GetScale() const
{
	return scale;
}

const glm::mat4 Transform::GetMatrix() const
{
	glm::mat4 matrix = glm::mat4(1.0f);

	glm::vec3 eulerAngles = GetRotation();

	if (eulerAngles.x != 0.0f)
	{
		matrix = glm::rotate(matrix, glm::radians(eulerAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (eulerAngles.y != 0.0f)
	{
		matrix = glm::rotate(matrix, glm::radians(eulerAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (eulerAngles.z != 0.0f)
	{
		matrix = glm::rotate(matrix, glm::radians(eulerAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	matrix = glm::translate(matrix, GetPosition());
	matrix = glm::scale(matrix, GetScale());

	return matrix;
}
