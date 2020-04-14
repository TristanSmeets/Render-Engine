#pragma once
#include "imgui.h"
#include "Core/Light.h"
#include "Core/Window.h"
#include "Core/Transform.h"
#include "Core/Camera.h"
#include "Rendering/Texture.h"
#include "Rendering/Material.h"
#include "Rendering/RenderComponent.h"
#include "Core/Actor.h"

class GUIHelper
{
public:
	GUIHelper();
	virtual ~GUIHelper();
	void Initialize(const Window& window);
	void Render();

private:
	void RenderLayout();
	void RenderFPS();
	void RenderText(const char* text, ...);
	void RenderInt(const char* name, int& value, int minimum, int maximum);
	void RenderFloat(const char* name, float& value, float minimum, float maximum);
	void RenderVec3(const char* name,const glm::vec3& vec3);
	void RenderColour(const glm::vec3& colour);
	void RenderTransform(const Transform& transform);
	void RenderFrustum(const Camera::Frustum& frustum);
	void RenderTexture(const Texture& texture);
	void RenderMaterial(const Material& material);
	void RenderRenderComponent(const RenderComponent& renderComponent);
	void RenderActor(const Actor& actor);
	void RenderCamera(const Camera& camera);
	void RenderLight(const Light& light);
	
	std::vector<Actor> actors;
	std::vector<Light> lights;
	Camera camera;
	Actor actor;
	Light light;
	Mesh sphereMesh;
	Material material = Material("Test");
	int aspectRatio = 2;
	float fieldOfViewDegrees = 45.0f;
};