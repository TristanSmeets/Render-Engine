#pragma once

#include "Core/Actor.h"

class Light : public Actor
{
public:
    Light(const std::string& name = "Light", const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& colour = glm::vec3(1.0f));
    virtual ~Light();

    void SetColour(const glm::vec3& colour);
    const glm::vec3& GetColour() const;
private:
    glm::vec3 colour;
};