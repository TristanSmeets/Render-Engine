#include "Rendererpch.h"
#include "Camera.h"
#include <gtx/rotate_vector.hpp>

Camera::Frustum::Frustum(float nearPlane, float farPlane, float fieldOfView, float aspectRatio) :
	NearPlaneCutoff(nearPlane), FarPlaneCutoff(farPlane), FieldOfView(fieldOfView), AspectRatio(aspectRatio)
{
}

Camera::Frustum & Camera::Frustum::operator=(const Frustum & rhs)
{
	NearPlaneCutoff = rhs.NearPlaneCutoff;
	FarPlaneCutoff = rhs.FarPlaneCutoff;
	FieldOfView = rhs.FieldOfView;
	AspectRatio = rhs.AspectRatio;
	return *this;
}

Camera::Camera() :
	Actor("Camera")
{
}

Camera::Camera(const glm::vec3 & position, const glm::vec3 & rotationInEulerAngles, const Frustum & frustum) :
	Actor("Camera", position, rotationInEulerAngles), frustum(frustum)
{
	EventQueue<KeyEvent>::AddListener(std::bind(&Camera::OnKeyEvent, this, std::placeholders::_1));
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
	MultiplySpeedWithDeltaTime(deltaTime);
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

const float & Camera::GetMoveSpeed() const
{
	return moveSpeed;
}

const float & Camera::GetRotationSpeed() const
{
	return rotationSpeed;
}

Camera & Camera::operator=(const Camera & rhs)
{
	frustum = rhs.frustum;
	return *this;
}

const glm::vec3 Camera::GetCameraSide() const
{
	return glm::normalize(glm::cross(GetCameraFront(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

const glm::vec3 Camera::GetCameraUp() const
{
	return glm::normalize(glm::cross(GetCameraSide(), GetCameraFront()));
}

void Camera::MultiplySpeedWithDeltaTime(float deltaTime)
{
	mSpeed = moveSpeed * deltaTime;
	rSpeed = rotationSpeed * deltaTime;
}

void Camera::OnKeyEvent(const KeyEvent & keyEvent)
{
	//Translation
	if (keyEvent.Key == KeyEvent::KEY::W && keyEvent.State == KeyEvent::STATE::PRESS)
	{
		transform.Translate(mSpeed * GetCameraFront());
	}
	if (keyEvent.Key == KeyEvent::KEY::S && keyEvent.State == KeyEvent::STATE::PRESS)
	{
		transform.Translate(-mSpeed * GetCameraFront());
	}
	if (keyEvent.Key == KeyEvent::KEY::A && keyEvent.State == KeyEvent::STATE::PRESS)
	{
		transform.Translate(-mSpeed * GetCameraSide());
	}
	if (keyEvent.Key == KeyEvent::KEY::D && keyEvent.State == KeyEvent::STATE::PRESS)
	{
		transform.Translate(mSpeed * GetCameraSide());
	}
	if (keyEvent.Key == KeyEvent::KEY::T && keyEvent.State == KeyEvent::STATE::PRESS)
	{
		transform.Translate(mSpeed * GetCameraUp());
	}
	if (keyEvent.Key == KeyEvent::KEY::G && keyEvent.State == KeyEvent::STATE::PRESS)
	{
		transform.Translate(-mSpeed * GetCameraUp());
	}
	//Rotation
	if (keyEvent.Key == KeyEvent::KEY::Q && keyEvent.State == KeyEvent::STATE::PRESS)
	{
		transform.Rotate(glm::vec3(0.0f, -rSpeed, 0.0f));
	}
	if (keyEvent.Key == KeyEvent::KEY::E && keyEvent.State == KeyEvent::STATE::PRESS)
	{
		transform.Rotate(glm::vec3(0.0f, rSpeed, 0.0f));
	}
	if (keyEvent.Key == KeyEvent::KEY::R && keyEvent.State == KeyEvent::STATE::PRESS)
	{
		transform.Rotate(glm::vec3(rSpeed, 0.0f, 0.0f));
	}
	if (keyEvent.Key == KeyEvent::KEY::F && keyEvent.State == KeyEvent::STATE::PRESS)
	{
		transform.Rotate(glm::vec3(-rSpeed, 0.0f, 0.0f));
	}
}
