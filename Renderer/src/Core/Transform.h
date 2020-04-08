#pragma once
#include "glm.hpp"

class Transform
{
public:
    Transform(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
    Transform(const Transform& rhs);
    ~Transform();
    void Translate(const glm::vec3& translation);
    void Rotate(const glm::vec3& eulerAngles);
    void Scale(const glm::vec3& scale);
    const glm::vec3 GetPosition() const;
    const glm::vec3 GetRotation() const;
    const glm::vec3 GetScale() const;
    const glm::mat4 GetMatrix() const;

private:
	glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
};