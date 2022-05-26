#include "Workspace.h"

#include <cstdio>

namespace AmaterasuDemo
{
	Workspace::Workspace(const std::string& name)
		: m_Name(name)
	{
		m_Tools.push_back(new Tool("TestTool1" + name));
		m_Tools.push_back(new Tool("TestTool2" + name));
		m_Tools.push_back(new Tool("TestTool3" + name));
	}

	void Workspace::Render()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiStyle oldStyle = style;

		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.FramePadding = ImVec2(8.0f, 8.0f);
		style.WindowPadding = ImVec2(0.0f, 0.0f);

		ImGuiWindowClass workspaceClass;
		workspaceClass.ClassId = ImHashStr("Workspace");
		workspaceClass.DockingAllowUnclassed = false;
		ImGui::SetNextWindowClass(&workspaceClass);
		char buffer[264];
		std::sprintf(buffer, "Workspace_%s", m_Name.c_str());
		ImGui::Begin(buffer, (bool*)0, ImGuiDockNodeFlags_None);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 p1 = ImGui::GetCursorScreenPos();
		ImVec2 p2 = p1 + ImVec2(ImGui::GetContentRegionAvail().x + (style.WindowPadding.x * 2), 3.0f);
		drawList->AddRectFilled(p1, p2, IM_COL32(36, 36, 36, 255));
		ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.0f, 3.0f));
	
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.FramePadding = ImVec2(4.0f, 4.0f);
		ImGuiWindowClass sceneToolClass;
		sceneToolClass.ClassId = ImHashStr("SceneToolClass");
		sceneToolClass.DockingAllowUnclassed = false;
		
		ImGui::DockSpace(ImGui::GetID(buffer), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None, &sceneToolClass);

		for (Tool* tool : m_Tools)
		{
			tool->Render();
		}

		ImGui::End();

		style = oldStyle;
	}
}