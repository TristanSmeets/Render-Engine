#include "Rendererpch.h"
#include "GUIHelper.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Utility/Filepath.h"
#include "Utility/MeshLoader.h"

GUIHelper::GUIHelper()
{
}

GUIHelper::~GUIHelper()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUIHelper::Initialize(const Window& window)
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

void GUIHelper::StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GUIHelper::EndFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIHelper::Render(Scene & scene)
{
	//ImGui::ShowDemoWindow();

	ImGui::Begin("Scene");
	RenderLayout(scene);
	ImGui::End();
}

void GUIHelper::Render(RenderTechnique & technique)
{
	ImGui::Begin("Deferred Parameters");
	ImGui::Separator();
	RenderDeferredParameters(technique.GetDeferredParameters());
	ImGui::Separator();
	//RenderPostProcessingParameters(technique.GetPostProcessing().GetParameters());
	ImGui::End();
}

void GUIHelper::RenderLayout(Scene & scene)
{
	ImGui::Columns(2);
	RenderText("Framerate");
	ImGui::NextColumn();
	RenderFPS();
	ImGui::NextColumn();
	ImGui::Separator();
	if (ImGui::TreeNode("Camera"))
	{
		ImGui::NextColumn();
		RenderCamera(scene.GetCamera());
		ImGui::TreePop();
		ImGui::NextColumn();
	}
	ImGui::Separator();
	if (ImGui::TreeNode("Light"))
	{
		const std::vector<Light>& lights = scene.GetLights();
		static int selectedLight = -1;
		for (int i = 0; i < lights.size(); ++i)
		{
			if (ImGui::Selectable(lights[i].GetName().c_str(), selectedLight == i))
			{
				selectedLight = i;
			}
		}
		ImGui::NextColumn();
		if (selectedLight != -1)
		{
			RenderLight(lights[selectedLight]);
		}
		ImGui::TreePop();
		ImGui::NextColumn();
	}
	ImGui::Separator();
	if (ImGui::TreeNode("Actors"))
	{
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
			RenderActor(actors[selectedActor]);
		}
		ImGui::TreePop();
		ImGui::NextColumn();
	}
}

void GUIHelper::RenderFPS()
{
	RenderText("%.1f FPS (%.2f ms/frame)", ImGui::GetIO().Framerate, (1000.0f / ImGui::GetIO().Framerate));
}

void GUIHelper::RenderText(const char* text, ...)
{
	va_list args;
	va_start(args, text);
	ImGui::TextV(text, args);
	va_end(args);
}

void GUIHelper::RenderTransform(const Transform & transform)
{
	RenderVec3("Position", transform.GetPosition());
	RenderVec3("Rotation", transform.GetRotation());
	RenderVec3("Scale", transform.GetScale());
}

void GUIHelper::RenderFrustum(const Camera::Frustum & frustum)
{
	RenderFloat("Near plane", (float&)frustum.NearPlaneCutoff, 0.0f, 5.0f);
	RenderFloat("Far plane", (float&)frustum.FarPlaneCutoff, 5.0f, 1000000.0f);
	ImGui::Combo("Aspect ratio", &aspectRatio, " 1:1\0 4:3\0 16:9\0");
	RenderFloat("Field of View", fieldOfViewDegrees, 60.0f, 120.0f);
}

void GUIHelper::RenderTexture(const Texture & texture)
{
	RenderText("Name: %s", texture.GetName().c_str());
	ImGui::Image((void*)(intptr_t)texture.GetID(), ImVec2(256, 256));
}

void GUIHelper::RenderMaterial(const Material & material)
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

void GUIHelper::RenderRenderComponent(RenderComponent & renderComponent)
{
	RenderText("Mesh:\n\t%s", renderComponent.GetMesh().GetName().c_str());
	RenderText("Material");
	//RenderADSParameters(renderComponent.GetADSParameters());
	RenderPBRParameters(renderComponent.GetPBRParameters());
	RenderMaterial(renderComponent.GetMaterial());
}

void GUIHelper::RenderActor(Actor & actor)
{
	//RenderText("Name: %s", actor.GetName());
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

void GUIHelper::RenderCamera(const Camera & camera)
{
	if (ImGui::TreeNode("Transform"))
	{
		RenderTransform(camera.GetTransform());
		ImGui::TreePop();
	}
	RenderFloat("Movement Speed", (float&)camera.GetMoveSpeed(), 0.0f, 100.0f);
	RenderFloat("Rotation Speed", (float&)camera.GetRotationSpeed(), 0.0f, 100.0f);
	if (ImGui::TreeNode("Frustum"))
	{
		RenderFrustum(camera.GetFrustum());
		ImGui::TreePop();
	}
}

void GUIHelper::RenderLight(const Light & light)
{
	if (ImGui::TreeNode("Transform"))
	{
		RenderTransform(light.GetTransform());
		ImGui::TreePop();
	}
	const Light::Parameters& parameters = light.GetParameters();
	RenderColour(parameters.Colour);
	RenderFloat("Constant", (float&)parameters.Constant, 0.1f, 2.0f);
	RenderFloat("Linear", (float&)parameters.Linear, 0.0f, 1.0f);
	RenderFloat("Quadratic", (float&)parameters.Quadratic, 0.0f, 2.0f);
	RenderFloat("Radius", (float&)parameters.Radius, 0.0f, 100.0f);
}

void GUIHelper::RenderDirectionalLight(const DirectionalLight & light)
{
	RenderLight(light);
	RenderVec3("Direction",light.GetFront());
}

void GUIHelper::RenderADSParameters(RenderComponent::ADSParameters & adsParameters)
{
	RenderFloat("Ambient Strength", (float&)adsParameters.AmbientStrength, 0.0f, 1.0f);
	RenderFloat("Material Shiniess", (float&)adsParameters.Shininess, 2.0f, 256.0f);
	ImGui::Checkbox("Is Transparent", &adsParameters.IsTransparent);
}

void GUIHelper::RenderPBRParameters(RenderComponent::PBRParameters & pbrParameters)
{
	RenderText("Non Metallic Reflection Colour");
	RenderColour(pbrParameters.NonMetallicReflectionColour);
	RenderFloat("Roughness", (float&)pbrParameters.Roughness, 0.0f, 1.0f);
	ImGui::Checkbox("Is Transparent", &pbrParameters.IsTransparent);
	ImGui::Checkbox("Using Smoothness", &pbrParameters.UsingSmoothness);
}

void GUIHelper::RenderDeferredParameters(RenderTechnique::DeferredParameters & deferredParameters)
{
	RenderInt("SSAO Occlusion Power", (int&)deferredParameters.OcclusionPower, 1, 200);
	RenderInt("SSAO Kernel Size", (int&)deferredParameters.KernelSize, 1, 64);
	RenderFloat("SSAO Radius", (float&)deferredParameters.Radius, 0.0f, 5.0f);
	RenderFloat("SSAO Bias", (float&)deferredParameters.Bias, 0.0f, 2.0f);
	ImGui::Separator();
	RenderADSParameters(deferredParameters.AdsParameters);
	RenderPBRParameters(deferredParameters.PbrParameters);
}

void GUIHelper::RenderColour(const glm::vec3& colour)
{
	ImGui::ColorEdit3("Colour", (float*)&colour, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
}

void GUIHelper::RenderVec3(const char * name, const glm::vec3 & vec3)
{
	ImGui::DragFloat3(name, (float*)&vec3, 0.01f, -1000.0f, 1000.0f, "%.2f");
}

void GUIHelper::RenderInt(const char * name, int & value, int minimum, int maximum)
{
	ImGui::DragInt(name, &value, 1.0f, minimum, maximum);
}

void GUIHelper::RenderFloat(const char * name, float & value, float minimum, float maximum)
{
	ImGui::DragFloat(name, &value, 0.1f, minimum, maximum, "%.2f");
}
