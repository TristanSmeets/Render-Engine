#pragma once

#include "Core/Actor.h"
#include "glm.hpp"
#include "GLFW/glfw3.h"

class Camera : public Actor
{
public:
	struct Frustum
	{
		const float NearPlaneCutoff;
		const float FarPlaneCutoff;
		const float FieldOfView;
		const float AspectRatio;

		Frustum(float nearPlane = 0.1f, float farPlane = 100.0f, float fieldOfView = glm::radians(45.0f), float aspectRatio = (16.0f / 9.0f));
	};

public:
	Camera(GLFWwindow* window, const glm::vec3& position = glm::vec3(0.0f, 0.0f, 5.0f), const glm::vec3& rotationInEulerAngles = glm::vec3(-90.0f, 0.0f, 0.0f), const Frustum& frustum = Frustum());
	virtual ~Camera();
	const glm::mat4 GetViewMatrix();
	const glm::mat4 GetProjectionMatrix();
	void Update(float deltaTime);
	const Frustum GetFrustum() const;
	const glm::vec3 GetCameraUp() const;
	const glm::vec3 GetCameraSide() const;
	const glm::vec3 GetCameraFront() const;

private:
	void ProcessKeyBoardInput(float deltaTime);

	GLFWwindow* window;
	Frustum frustum = Frustum();
	const float moveSpeed = 2.5f;
	const float rotationSpeed = 50.0f;
};