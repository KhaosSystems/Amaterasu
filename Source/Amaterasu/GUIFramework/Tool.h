#pragma once

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <concepts>
#include <string>
#include <typeinfo>
#include <iostream>

namespace Amaterasu
{
    class ITool
    {
    public:
        virtual void BeginRender() = 0;
        virtual void Render() = 0;
        virtual void EndRender() = 0;
        virtual const std::type_info& GetWorkspaceTypeInfo() const = 0;
    };

    template<typename WorkspaceType, typename ToolType>
    class Tool : public ITool
	{
    public:
        Tool(const std::string& displayName)
            : m_Name(typeid(ToolType).name()), m_DisplayName(displayName), m_WorkspaceTypeName(typeid(WorkspaceType).name())
        {
        }

        virtual void Initialize(WorkspaceType* workspace) 
        {
            m_Workspace = workspace;
        }

        virtual void BeginRender() override
        {
            ImGuiStyle& style = ImGui::GetStyle();
            m_OldStyle = style;

            ImGuiWindowClass toolWindowClass{};
            toolWindowClass.ClassId = ImHashStr(std::string("Node Graph Demo Workspace###WindowClass_" + m_WorkspaceTypeName).c_str());
            toolWindowClass.DockingAllowUnclassed = false;

            style.WindowMenuButtonPosition = ImGuiDir_None;
            style.FramePadding = ImVec2(4.0f, 4.0f);

            ImGui::SetNextWindowClass(&toolWindowClass);
            std::string windowName = m_DisplayName + "###ToolWindow_" + m_Name;
            ImGui::Begin(windowName.c_str());
        }

        virtual void Render() override
        {
        }

        virtual void EndRender() override
        {
            ImGui::End();
            
            ImGuiStyle& style = ImGui::GetStyle();
            style = m_OldStyle;
        }

        virtual const std::type_info& GetWorkspaceTypeInfo() const override { return typeid(WorkspaceType); }

    protected:
        WorkspaceType* m_Workspace = nullptr;
        const std::string m_Name;
        const std::string m_DisplayName;
        const std::string m_WorkspaceTypeName;

        // TMP
        ImGuiStyle m_OldStyle;
	};
}