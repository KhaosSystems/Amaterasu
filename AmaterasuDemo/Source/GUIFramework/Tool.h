#pragma once

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <concepts>
#include <string>
#include <iostream>

namespace AmaterasuDemo
{
    class ITool
    {
    public:
        virtual void Render() = 0;
    };

    template<typename WorkspaceT, typename ToolT>
    class Tool : public ITool
	{
    public:
        Tool(const std::string& displayName)
            : m_Name(typeid(ToolT).name()), m_DisplayName(displayName), m_WorkspaceTypeName(typeid(WorkspaceT).name())
        {
        }

        virtual void Render() override
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

    protected:
        const std::string m_Name;
        const std::string m_DisplayName;
        const std::string m_WorkspaceTypeName;
	};
}