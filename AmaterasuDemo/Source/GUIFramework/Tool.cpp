#include "Tool.h"

namespace AmaterasuDemo
{
	Tool::Tool(const std::string& name)
		: m_Name(name)
	{
	}

	void Tool::Render()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiStyle oldStyle = style;

		ImGuiWindowClass sceneToolClass;
		sceneToolClass.ClassId = ImHashStr("SceneToolClass");
		sceneToolClass.DockingAllowUnclassed = false;

		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.FramePadding = ImVec2(4.0f, 4.0f);

		ImGui::SetNextWindowClass(&sceneToolClass);
		ImGui::Begin(m_Name.c_str());

		ImGui::End();

		style = oldStyle;
	}
}