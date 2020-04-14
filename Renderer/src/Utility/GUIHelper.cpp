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

	const std::vector<Mesh> sphereMeshes = MeshLoader::LoadModel(Filepath::Mesh + "sphere.obj");
	sphereMesh = sphereMeshes[0];
	//renderComponent.SetMesh(sphereMesh);

	material.AddTexture(Texture::Albedo, Filepath::Texture + "Aluminium/Albedo.png", true);
	material.AddTexture(Texture::Normal, Filepath::Texture + "Aluminium/Normal.png");
	material.AddTexture(Texture::Metallic, Filepath::Texture + "Aluminium/Metallic.png");
	material.AddTexture(Texture::Roughness, Filepath::Texture + "Aluminium/Roughness.png");
	material.AddTexture(Texture::AmbientOcclusion, Filepath::Texture + "Aluminium/Mixed_AO.png");
	//renderComponent.SetMaterial(material);

	actor.GetRenderComponent().SetMaterial(material);
	actor.GetRenderComponent().SetMesh(sphereMesh);

	for (int i = 0; i < 5; ++i)
	{
		actor.SetName(std::string("Actor#") + std::to_string(i));
		actors.push_back(actor);
	}

	for (int i = 0; i < 3; ++i)
	{
		std::string name = std::string("Light#") + std::to_string(i);
		light.SetName(name);
		lights.push_back(light);
	}
}

void GUIHelper::Render()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

	ImGui::Begin("Scene");
	RenderLayout();
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIHelper::RenderLayout()
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
		RenderCamera(camera);
		ImGui::TreePop();
		ImGui::NextColumn();
	}
	ImGui::Separator();
	if (ImGui::TreeNode("Light"))
	{
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
		static int selectedActor = -1;
		for (int n = 0; n < actors.size(); n++)
		{
			if (ImGui::Selectable(actors[n].GetName().c_str(), selectedActor == n))
			{
				selectedActor = n;
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
	RenderText("Name: %s", texture.GetName());
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

void GUIHelper::RenderRenderComponent(const RenderComponent & renderComponent)
{
	RenderText("Mesh:\n\t%s", renderComponent.GetMesh().GetName());
	RenderText("Material");
	RenderMaterial(renderComponent.GetMaterial());
}

void GUIHelper::RenderActor(const Actor & actor)
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
	RenderColour(light.GetColour());
}

void GUIHelper::RenderColour(const glm::vec3& colour)
{
	ImGui::ColorEdit3("Colour", (float*)&colour, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
}

void GUIHelper::RenderVec3(const char * name, const glm::vec3 & vec3)
{
	ImGui::DragFloat3(name, (float*)&vec3, 0.1f, -1000.0f, 1000.0f, "%.2f");
}

void GUIHelper::RenderInt(const char * name, int & value, int minimum, int maximum)
{
	ImGui::DragInt(name, &value, 1.0f, minimum, maximum);
}

void GUIHelper::RenderFloat(const char * name, float & value, float minimum, float maximum)
{
	ImGui::DragFloat(name, &value, 0.1f, minimum, maximum, "%.2f");
}
