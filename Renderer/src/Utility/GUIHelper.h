#pragma once
#include "imgui.h"
#include "Core/Scene.h"
#include "Core/Actor.h"
#include "Core/Light.h"
#include "Core/Window.h"
#include "Core/Transform.h"
#include "Core/Camera.h"
#include "Rendering/Texture.h"
#include "Rendering/Material.h"
#include "Rendering/RenderComponent.h"
#include "Technique/RenderTechnique.h"
#include "PostProcessing/PostProcessing.h"

class GUIHelper
{
public:
	GUIHelper();
	virtual ~GUIHelper();
	void Initialize(const Window& window);
	void StartFrame();
	void EndFrame();
	void Render(const Scene& scene);
	void Render(const RenderTechnique& technique);


private:
	void RenderLayout(const Scene& scene);
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
	void RenderDirectionalLight(const DirectionalLight& light);
	void RenderADSParameters(const RenderComponent::ADSParameters& adsParameters);
	void RenderPBRParameters(const RenderComponent::PBRParameters& pbrParameters);
	void RenderDeferredParameters(const RenderTechnique::DeferredParameters& deferredParameters);
	void RenderPostProcessingParameters(const PostProcessing::Parameters& postProcessingParameters);

	int aspectRatio = 2;
	float fieldOfViewDegrees = 45.0f;
};