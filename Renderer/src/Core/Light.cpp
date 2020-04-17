#include "Rendererpch.h"

#include "Light.h"

Light::Light(const std::string& name, const glm::vec3 & position, const glm::vec3 & colour) :
    Actor(name, position, glm::vec3(0.0f), glm::vec3(0.5f)), colour(colour)
{
}

Light::~Light()
{
    Actor::~Actor();
}

void Light::SetColour(const glm::vec3 & colour)
{
    this->colour = colour;
}

const glm::vec3 & Light::GetColour() const
{
    return colour;
}
