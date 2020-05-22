#pragma once

#include "Core/Actor.h"

class Light : public Actor
{
public:
	struct Parameters
	{
		glm::vec3 Colour;
		float Constant;
		float Linear;
		float Quadratic;
		float Radius;
		Parameters(glm::vec3 colour = glm::vec3(1.0f), float constant = 1.0f, float linear = 0.14f, float quadratic = 0.07f, float radius = 50.0f);
		Parameters& operator=(const Parameters& rhs);
	};

public:
    Light(const std::string& name = "Light", const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& colour = glm::vec3(1.0f));
    Light(const std::string& name, const glm::vec3& position, const Parameters& parameters);
    virtual ~Light();

    void SetColour(const glm::vec3& colour);
    const glm::vec3& GetColour() const;
	const Parameters& GetParameters() const;
private:
	Parameters parameters;
};