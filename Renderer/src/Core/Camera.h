#pragma once

#include "Core/Actor.h"
#include "glm.hpp"
#include "GLFW/glfw3.h"
#include "Event/KeyEvent.h"
#include "Event/EventQueue.h"

class Camera : public Actor
{
public:
	struct Frustum
	{
		float NearPlaneCutoff;
		float FarPlaneCutoff;
		float FieldOfView;
		float AspectRatio;

		Frustum(float nearPlane = 0.1f, float farPlane = 150.0f, float fieldOfView = glm::radians(45.0f), float aspectRatio = (16.0f / 9.0f));
		Frustum& operator=(const Frustum& rhs);
	};

public:
	Camera();
	Camera(const glm::vec3& position = glm::vec3(17.75f, 8.10f, 29.0f), const glm::vec3& rotationInEulerAngles = glm::vec3(-8.25f, -120.9f, 0.0f), const Frustum& frustum = Frustum());
	virtual ~Camera();
	const glm::mat4 GetViewMatrix() const;
	const glm::mat4 GetProjectionMatrix() const;
	void Update(float deltaTime);
	const Frustum GetFrustum() const;
	const glm::vec3 GetCameraUp() const;
	const glm::vec3 GetCameraSide() const;
	const glm::vec3 GetCameraFront() const;
	const float& GetMoveSpeed() const;
	const float& GetRotationSpeed() const;
	Camera& operator=(const Camera& rhs);

private:
	void MultiplySpeedWithDeltaTime(float deltaTime);
	void OnKeyEvent(const KeyEvent& keyEvent);
	Frustum frustum = Frustum();
	float moveSpeed = 25.0f;
	float mSpeed = 0.0f;
	float rotationSpeed = 50.0f;
	float rSpeed = 0.0f;
};