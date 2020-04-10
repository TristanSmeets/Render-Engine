#include "Rendererpch.h"
#include "GUIHelper.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

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
	if (ImGui::TreeNode(""))
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
	RenderText("Int: ");
	ImGui::NextColumn();
	RenderInt("Width", test, 0, 1920);
	
}

void GUIHelper::RenderFPS()
{
	RenderText("%.1f FPS (%.2f ms/frame) ", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
}

void GUIHelper::RenderText(const char* text, ...)
{
	va_list args;
	va_start(args, text);
	ImGui::Text(text, args);
	va_end(args);
}

void GUIHelper::RenderTransform(const Transform & transform)
{
	RenderVec3("Position", transform.GetPosition());
	RenderVec3("Rotation", transform.GetRotation());
	RenderVec3("Scale", transform.GetScale());
}

void GUIHelper::RenderColour(const glm::vec3& colour)
{
	ImGui::ColorEdit3("Colour", (float*)&colour, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
}

void GUIHelper::RenderVec3(const char * name, const glm::vec3 & vec3)
{
	ImGui::DragFloat3(name, (float*)&vec3, 0.1f,-1000.0f,1000.0f,"%.2f");
}

void GUIHelper::RenderInt(const char * name, int & value, int minimum, int maximum)
{
	ImGui::DragInt(name, &value, 1.0f, minimum, maximum);
}
