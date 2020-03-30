#include "Transform.h"

#include <gtx/transform.hpp>

Transform::Transform(const glm::vec3 & position, const glm::vec3 & rotation, const glm::vec3 & scale)
{
    Rotate(rotation);
    Translate(position);
    Scale(scale);
}

Transform::Transform(const Transform & rhs) :
    rotation(rhs.rotation), scale(rhs.scale), matrix(rhs.matrix)
{
}

Transform::~Transform()
{
}

void Transform::Translate(const glm::vec3 & translation)
{
    matrix = glm::translate(matrix, translation);
}

void Transform::Rotate(const glm::vec3 & eulerAngles)
{
    rotation = eulerAngles;

    if (rotation.x != 0.0f)
    {
        matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (rotation.y != 0.0f)
    {
        matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (rotation.z != 0.0f)
    {
        matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    }
}

void Transform::Scale(const glm::vec3 & scale)
{
    this->scale = scale;
    matrix = glm::scale(matrix, this->scale);
}

const glm::vec3 & Transform::GetPosition() const
{
    return matrix[3];
}

const glm::vec3 & Transform::GetRotation() const
{
    return rotation;
}

const glm::vec3 & Transform::GetScale() const
{
    return scale;
}

const glm::mat4 & Transform::GetMatrix() const
{
    return matrix;
}
