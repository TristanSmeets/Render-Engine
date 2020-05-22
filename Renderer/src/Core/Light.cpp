#include "Rendererpch.h"

#include "Light.h"

Light::Light(const std::string& name, const glm::vec3 & position, const glm::vec3 & colour) :
    Actor(name, position, glm::vec3(0.0f), glm::vec3(0.5f))
{
	parameters.Colour = colour;
}

Light::Light(const std::string & name, const glm::vec3 & position, const Parameters & parameters) :
	Actor(name, position, glm::vec3(0.0f), glm::vec3(0.5f)), parameters(parameters)
{
}

Light::~Light()
{
    Actor::~Actor();
}

void Light::SetColour(const glm::vec3 & colour)
{
    parameters.Colour = colour;
}

const glm::vec3 & Light::GetColour() const
{
    return parameters.Colour;
}

const Light::Parameters & Light::GetParameters() const
{
	return parameters;
}

Light::Parameters::Parameters(glm::vec3 colour, float constant, float linear, float quadratic, float radius) :
	Colour(colour), Constant(constant), Linear(linear), Quadratic(quadratic), Radius(radius)
{
}

Light::Parameters & Light::Parameters::operator=(const Parameters & rhs)
{
	Colour = rhs.Colour;
	Constant = rhs.Constant;
	Linear = rhs.Linear;
	Quadratic = rhs.Quadratic;
	Radius = rhs.Radius;
	return *this;
}
