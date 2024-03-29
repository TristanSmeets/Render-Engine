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
#include "Utility/GaussianBlur.h"

class UserInterface
{
public:
	UserInterface();
	virtual ~UserInterface();
	void Initialize(const Window& window);
	void StartFrame();
	void EndFrame();
	void Render(Scene& scene);
	void Render(RenderTechnique& technique);


private:
	void RenderLayout(Scene& scene);
	void RenderFPS();
	void RenderText(const char* text, ...);
	void RenderInt(const char* name, int& value, int minimum, int maximum);
	void RenderFloat(const char* name, float& value, float minimum, float maximum);
	void RenderVec2(const char* name, const glm::vec2& vec2);
	void RenderVec3(const char* name,const glm::vec3& vec3);
	void RenderColour(const glm::vec3& colour);
	void RenderTransform(const Transform& transform);
	void RenderFrustum(const Camera::Frustum& frustum);
	void RenderTexture(const Texture& texture);
	void RenderMaterial(const Material& material);
	void RenderRenderComponent(RenderComponent& renderComponent);
	void RenderActor(Actor& actor);
	void RenderCamera(const Camera& camera);
	void RenderLight(const Light& light);
	void RenderDeferredParameters(RenderTechnique::DeferredParameters& deferredParameters);
	void RenderPostProcessingParameters(PostProcessing::Parameters& ppParameters);
	/*void RenderFXAAParameters(FXAA::Parameters& fxaaParamters);
	void RenderBloomParameters(Bloom::Parameters& bloomParameters);*/
	void RenderSSAOParameters(SSAO::Parameters& ssoaParameters);
	void RenderGaussianBlurParameters(GaussianBlur::Parameters& blurParameters);


	int aspectRatio = 2;
	float fieldOfViewDegrees = 45.0f;
};
