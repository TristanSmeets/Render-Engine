#pragma once

#include "Core/Actor.h"

class Light : public Actor
{
public:
	struct Parameters
	{
		glm::vec3 Colour;
		float Radius;
		float CircleRadius = 10.0f;
		glm::vec3 StartPosition = glm::vec3(0.0f);
		bool isRotatingClockwise = false;
		Parameters(glm::vec3 colour = glm::vec3(1.0f), float radius = 50.0f);
		Parameters& operator=(const Parameters& rhs);
	};

public:
    Light(const std::string& name = "Light", const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& colour = glm::vec3(1.0f));
    Light(const std::string& name, const glm::vec3& position, const Parameters& parameters);
    virtual ~Light();

    void SetColour(const glm::vec3& colour);
	void Update(float deltaTime);
    const glm::vec3& GetColour() const;
	const Parameters& GetParameters() const;
private:
	Parameters parameters;
	float counter = 0;
	glm::vec3 startPosition = glm::vec3(0.0f);
};