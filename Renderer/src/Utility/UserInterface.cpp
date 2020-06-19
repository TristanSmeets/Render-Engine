#include "Rendererpch.h"
#include "UserInterface.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Utility/Filepath.h"
#include "Utility/MeshLoader.h"

UserInterface::UserInterface()
{
}

UserInterface::~UserInterface()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UserInterface::Initialize(const Window& window)
{
	//Setup Dear ImGUI context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	//Setup dear ImGui style
	ImGui::StyleColorsDark();

	//Setup platform/renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window.GetGLFWwindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void UserInterface::StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void UserInterface::EndFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UserInterface::Render(Scene & scene)
{
	ImGui::Begin("Properties");
	ImGui::End();
	ImGui::Begin("Scene");
	RenderLayout(scene);
	ImGui::End();
}

void UserInterface::Render(RenderTechnique & technique)
{
	ImGui::Begin("Post Processing");
	RenderDeferredParameters(technique.GetDeferredParameters());
	ImGui::End();
}

void UserInterface::RenderLayout(Scene & scene)
{
	
	ImGui::Columns(2);
	RenderText("Framerate");
	ImGui::NextColumn();
	RenderFPS();
	ImGui::NextColumn();
	ImGui::Separator();
	static bool selectedCamera = false;
	static bool selectedLights = false;
	static bool selectedActors = false;

	ImGui::Selectable("Camera", &selectedCamera);
	if (selectedCamera)
	{
		selectedLights = false;
		selectedActors = false;
		ImGui::Begin("Properties");
		RenderCamera(scene.GetCamera());
		ImGui::End();
	}

	ImGui::Separator();
	
	ImGui::Selectable("Lights", &selectedLights);
	if (selectedLights)
	{
		selectedCamera = false;
		selectedActors = false;
		ImGui::NextColumn();
		RenderInt("Lights", (int&)scene.GetNumberOfLights(), 0, 10);
		const std::vector<Light>& lights = scene.GetLights();
		static int selectedLight = -1;
		for (unsigned int i = 0; i < scene.GetNumberOfLights(); ++i)
		{
			if (ImGui::Selectable(lights[i].GetName().c_str(), selectedLight == i))
			{
				selectedLight = i;
			}
		}
		ImGui::NextColumn();
		if (selectedLight != -1)
		{
			ImGui::Begin("Properties");
			RenderLight(lights[selectedLight]);
			ImGui::End();
		}
	}
	ImGui::Separator();
	ImGui::Selectable("Actors", &selectedActors);
	if (selectedActors)
	{
		selectedCamera = false;
		selectedLights = false;
		ImGui::NextColumn();
		std::vector<Actor>& actors = scene.GetActors();
		static int selectedActor = -1;
		for (int i = 0; i < actors.size(); i++)
		{
			if (ImGui::Selectable(actors[i].GetName().c_str(), selectedActor == i))
			{
				selectedActor = i;
			}
		}
		ImGui::NextColumn();
		if (selectedActor != -1)
		{
			ImGui::Begin("Properties");
			RenderActor(actors[selectedActor]);
			ImGui::End();
		}
		ImGui::NextColumn();
	}
}

void UserInterface::RenderFPS()
{
	RenderText("%.1f FPS (%.2f ms/frame)", ImGui::GetIO().Framerate, (1000.0f / ImGui::GetIO().Framerate));
}

void UserInterface::RenderText(const char* text, ...)
{
	va_list args;
	va_start(args, text);
	ImGui::TextV(text, args);
	va_end(args);
}

void UserInterface::RenderTransform(const Transform & transform)
{
	RenderVec3("Position", transform.GetPosition());
	RenderVec3("Rotation", transform.GetRotation());
	RenderVec3("Scale", transform.GetScale());
}

void UserInterface::RenderFrustum(const Camera::Frustum & frustum)
{
	RenderFloat("Near plane", (float&)frustum.NearPlaneCutoff, 0.0f, 5.0f);
	RenderFloat("Far plane", (float&)frustum.FarPlaneCutoff, 5.0f, 1000000.0f);
}

void UserInterface::RenderTexture(const Texture & texture)
{
	RenderText("Name: %s", texture.GetName().c_str());
	ImGui::Image((void*)(intptr_t)texture.GetID(), ImVec2(256, 256));
}

void UserInterface::RenderMaterial(const Material & material)
{
	if (ImGui::TreeNode(material.GetName().c_str()))
	{
		for (int i = 0; i < Texture::Count; ++i)
		{
			RenderText(Texture::TypeToString((Texture::Type)i).c_str());
			RenderTexture(material.GetTexture((Texture::Type)i));
		}
		ImGui::TreePop();
	}
}

void UserInterface::RenderRenderComponent(RenderComponent & renderComponent)
{
	RenderText("Mesh:\n\t%s", renderComponent.GetMesh().GetName().c_str());
	RenderText("Material");
	RenderPBRParameters(renderComponent.GetPBRParameters());
	RenderMaterial(renderComponent.GetMaterial());
}

void UserInterface::RenderActor(Actor & actor)
{
	if (ImGui::TreeNode("Transform"))
	{
		RenderTransform(actor.GetTransform());
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Render Component"))
	{
		RenderRenderComponent(actor.GetRenderComponent());
		ImGui::TreePop();
	}
}

void UserInterface::RenderCamera(const Camera & camera)
{
	if (ImGui::TreeNode("Transform"))
	{
		RenderTransform(camera.GetTransform());
		ImGui::TreePop();
	}
	RenderFloat("Movement Speed", (float&)camera.GetMoveSpeed(), 0.0f, 100.0f);
	RenderFloat("Rotation Speed", (float&)camera.GetRotationSpeed(), 0.0f, 100.0f);
}

void UserInterface::RenderLight(const Light & light)
{
	if (ImGui::TreeNode("Transform"))
	{
		RenderTransform(light.GetTransform());
		ImGui::TreePop();
	}
	const Light::Parameters& parameters = light.GetParameters();
	RenderColour(parameters.Colour);
	RenderFloat("Radius", (float&)parameters.Radius, 0.0f, 100.0f);
}

void UserInterface::RenderADSParameters(RenderComponent::ADSParameters & adsParameters)
{
	RenderFloat("Ambient Strength", (float&)adsParameters.AmbientStrength, 0.0f, 1.0f);
	RenderFloat("Material Shiniess", (float&)adsParameters.Shininess, 2.0f, 256.0f);
	ImGui::Checkbox("Is Transparent", &adsParameters.IsTransparent);
}

void UserInterface::RenderPBRParameters(RenderComponent::PBRParameters & pbrParameters)
{
	RenderFloat("Roughness", (float&)pbrParameters.Roughness, 0.0f, 1.0f);
	ImGui::Checkbox("Is Transparent", &pbrParameters.IsTransparent);
	ImGui::Checkbox("Using Smoothness", &pbrParameters.UsingSmoothness);
}

void UserInterface::RenderDeferredParameters(RenderTechnique::DeferredParameters & deferredParameters)
{
	RenderSSAOParameters(deferredParameters.SsaoParameters);
	ImGui::Separator();
	RenderBloomParameters(deferredParameters.BloomParameters);
	ImGui::Separator();
	RenderDOFParameters(deferredParameters.DofParamaters);
}

void UserInterface::RenderFXAAParameters(FXAA::Parameters & fxaaParameters)
{
	RenderFloat("Span Max", (float&)fxaaParameters.SpanMax, 0.0f, 16.0f);
	RenderFloat("Reduce Minimum", (float&)fxaaParameters.ReduceMinumum, 0.0f, 1.0f);
	RenderFloat("Reduce Multiplier", (float&)fxaaParameters.ReduceMultiplier, .01f, 1.0f);
}

void UserInterface::RenderBloomParameters(Bloom::Parameters & bloomParameters)
{
	RenderFloat("Gamma Correction", (float&)bloomParameters.GammaCorrection, 0.1f, 3.0f);
	RenderFloat("Exposure", (float&)bloomParameters.Exposure, 0.1f, 2.0f);
}

void UserInterface::RenderDOFParameters(DepthOfField::Parameters & dofParameters)
{
	RenderFloat("Focal Distance", (float&)dofParameters.FocalDistance, 0.0f, 2.0f);
	RenderFloat("Focal Range", (float&)dofParameters.FocalRange, 0.0f, 1.0f);
	RenderFloat("Focal Range Cutoff", (float&)dofParameters.RangeCutoff, 0.0f, 1.0f);
	RenderInt("DoF Lod", (int&)dofParameters.Lod, 0, 7);
}

void UserInterface::RenderSSAOParameters(SSAO::Parameters & ssoaParameters)
{
	RenderInt("SSAO Occlusion Power", (int&)ssoaParameters.OcclusionPower, 1, 200);
	RenderInt("SSAO Kernel Size", (int&)ssoaParameters.KernelSize, 1, 64);
	RenderFloat("SSAO Radius", (float&)ssoaParameters.Radius, 0.0f, 5.0f);
	RenderFloat("SSAO Bias", (float&)ssoaParameters.Bias, 0.0f, 2.0f);
}

void UserInterface::RenderColour(const glm::vec3& colour)
{
	ImGui::ColorEdit3("Colour", (float*)&colour, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
}

void UserInterface::RenderVec3(const char * name, const glm::vec3 & vec3)
{
	ImGui::DragFloat3(name, (float*)&vec3, 0.01f, -1000.0f, 1000.0f, "%.2f");
}

void UserInterface::RenderInt(const char * name, int & value, int minimum, int maximum)
{
	ImGui::DragInt(name, &value, 1.0f, minimum, maximum);
}

void UserInterface::RenderFloat(const char * name, float & value, float minimum, float maximum)
{
	ImGui::DragFloat(name, &value, 0.1f, minimum, maximum, "%.2f");
}
