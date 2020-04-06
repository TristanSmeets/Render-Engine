#include "Rendererpch.h"
#include "Camera.h"
#include <gtx/rotate_vector.hpp>

Camera::Frustum::Frustum(float nearPlane, float farPlane, float fieldOfView, float aspectRatio) :
	NearPlaneCutoff(nearPlane), FarPlaneCutoff(farPlane), FieldOfView(fieldOfView), AspectRatio(aspectRatio)
{
}

Camera::Camera(GLFWwindow * window, const glm::vec3 & position, const glm::vec3 & rotationInEulerAngles, const Frustum & frustum) :
	Actor("Camera", position, rotationInEulerAngles), frustum(frustum), window(window)
{
}

Camera::~Camera()
{
	Actor::~Actor();
}

const glm::mat4 Camera::GetViewMatrix() const
{
	glm::vec3 position = GetWorldPosition();
	return glm::lookAt(position, position + GetCameraFront(), GetCameraUp());
}

const glm::mat4 Camera::GetProjectionMatrix() const
{
	return glm::perspective(frustum.FieldOfView, frustum.AspectRatio, frustum.NearPlaneCutoff, frustum.FarPlaneCutoff);
}

void Camera::Update(float deltaTime)
{
	ProcessKeyBoardInput(deltaTime);
}

const Camera::Frustum Camera::GetFrustum() const
{
	return frustum;
}

const glm::vec3 Camera::GetCameraFront() const
{
	glm::vec3 eulerAngles = transform.GetRotation();
	glm::vec3 direction;
	direction.x = cos(glm::radians(eulerAngles.y)) * cos(glm::radians(eulerAngles.x));
	direction.y = sin(glm::radians(eulerAngles.x));
	direction.z = sin(glm::radians(eulerAngles.y)) * cos(glm::radians(eulerAngles.x));
	return glm::normalize(direction);
}

const glm::vec3 Camera::GetCameraSide() const
{
	return glm::normalize(glm::cross(GetCameraFront(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

const glm::vec3 Camera::GetCameraUp() const
{
	return glm::normalize(glm::cross(GetCameraSide(), GetCameraFront()));
}

void Camera::ProcessKeyBoardInput(float deltaTime)
{
	float mSpeed = moveSpeed * deltaTime;
	float rSpeed = rotationSpeed * deltaTime;
	//Translation
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		transform.Translate(mSpeed * GetCameraFront());
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		transform.Translate(mSpeed * GetCameraFront());
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		transform.Translate(glm::normalize(glm::cross(GetCameraFront(), GetCameraUp())) * mSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		transform.Translate(glm::normalize(glm::cross(GetCameraFront(), GetCameraUp())) * mSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		transform.Translate(mSpeed * GetCameraUp());
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		transform.Translate(mSpeed * GetCameraUp());
	}
	//Rotation
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		transform.Rotate(glm::vec3(0.0f, rSpeed, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		transform.Rotate(glm::vec3(0.0f, -rSpeed, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		transform.Rotate(glm::vec3(rSpeed, 0.0f, 0.0f));
	}
	if (glfwGetKey(window,GLFW_KEY_F) == GLFW_PRESS)
	{
		transform.Rotate(glm::vec3(-rSpeed, 0.0f, 0.0f));
	}
}
