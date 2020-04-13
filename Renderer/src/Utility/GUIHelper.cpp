#include "Rendererpch.h"
#include "GUIHelper.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Utility/Filepath.h"

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

void GUIHelper::Render()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

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
	RenderText("Name");
	ImGui::NextColumn();
	RenderText("Tristan");
	ImGui::NextColumn();
	ImGui::Separator();
	RenderText("Transform");
	ImGui::NextColumn();
	if (ImGui::TreeNode("Transform"))
	{
		RenderTransform(testTransform);
		ImGui::TreePop();
	}
	ImGui::NextColumn();
	ImGui::Separator();
	RenderText("Colour");
	ImGui::NextColumn();
	RenderColour(light.GetColour());
	ImGui::NextColumn();
	ImGui::Separator();
	RenderText("Frustum");
	ImGui::NextColumn();
	if (ImGui::TreeNode("Frustum"))
	{
		RenderFrustum(frustum);
		ImGui::TreePop();
	}
	ImGui::NextColumn();
	ImGui::Separator();
	RenderText("Material", material.GetName());
	ImGui::NextColumn();
	RenderMaterial(material);

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
