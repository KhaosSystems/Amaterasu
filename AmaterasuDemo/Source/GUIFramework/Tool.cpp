#include "Tool.h"

#include "Workspace.h"

#include <iostream>

namespace AmaterasuDemo
{
    ITool::ITool(const std::string& name, const std::string& displayName, const std::string& workspaceTypeName)
        : m_Name(name), m_DisplayName(displayName), m_WorkspaceTypeName(workspaceTypeName)
	{
        std::cout << m_Name << std::endl;
        std::cout << m_DisplayName << std::endl;
	}

    void ITool::Render()
	{
        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiStyle oldStyle = style;

        ImGuiWindowClass toolWindowClass{};
        toolWindowClass.ClassId = ImHashStr(std::string("Node Graph Demo Workspace###WindowClass_" + m_WorkspaceTypeName).c_str());
        toolWindowClass.DockingAllowUnclassed = false;

        style.WindowMenuButtonPosition = ImGuiDir_None;
        style.FramePadding = ImVec2(4.0f, 4.0f);

        ImGui::SetNextWindowClass(&toolWindowClass);
        std::string windowName = m_DisplayName + "###ToolWindow_" + m_Name;
        ImGui::Begin(windowName.c_str());

        ImGui::End();

        style = oldStyle;
	}
}