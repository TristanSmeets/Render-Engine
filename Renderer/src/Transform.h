#pragma once
#include "glm.hpp"

class Transform
{
public:
    Transform();
    Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
    Transform(const Transform& rhs);
    ~Transform();
    void Translate(const glm::vec3& translation);
    void Rotate(const glm::vec3& eulerAngles);
    void Scale(const glm::vec3& scale);
    const glm::vec3& GetPosition() const;
    const glm::vec3& GetRotation() const;
    const glm::vec3& GetScale() const;
    const glm::mat4& GetMatrix() const;

private:
    glm::mat4 matrix = glm::mat4(1.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
};