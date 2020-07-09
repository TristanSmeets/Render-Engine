#include "Rendererpch.h"

#include "Light.h"

Light::Light(const std::string& name, const glm::vec3 & position, const glm::vec3 & colour) :
    Actor(name, position, glm::vec3(0.0f), glm::vec3(0.5f)), startPosition(position)
{
	parameters.Colour = colour;
}

Light::Light(const std::string & name, const glm::vec3 & position, const Parameters & parameters) :
	Actor(name, position, glm::vec3(0.0f), glm::vec3(0.5f)), parameters(parameters), startPosition(position)
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

void Light::Update(float deltaTime)
{
	counter += deltaTime * .25f;
	if (counter >= 360)
	{
		counter = 0;
	}

	//transform.SetPosition(glm::vec3(cos(counter) * parameters.CircleRadius, 0, sin(counter)* parameters.CircleRadius) + parameters.StartPosition);
	if (!parameters.isRotatingClockwise)
	{
		transform.SetPosition(glm::vec3(cos(-counter) * parameters.CircleRadius, sin(-counter), sin(-counter)* parameters.CircleRadius) + parameters.StartPosition);
	}
	else
	{
		transform.SetPosition(glm::vec3(cos(counter) * parameters.CircleRadius, cos(-counter), sin(counter)* parameters.CircleRadius) + parameters.StartPosition);
	}
}

const glm::vec3 & Light::GetColour() const
{
    return parameters.Colour;
}

const Light::Parameters & Light::GetParameters() const
{
	return parameters;
}

Light::Parameters::Parameters(glm::vec3 colour, float radius) :
	Colour(colour), Radius(radius)
{
}

Light::Parameters & Light::Parameters::operator=(const Parameters & rhs)
{
	Colour = rhs.Colour;
	Radius = rhs.Radius;
	return *this;
}
