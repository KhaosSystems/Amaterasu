#pragma once

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <string>
#include <vector>

namespace AmaterasuDemo
{
    class Workspace
    {
    public:
        Workspace(const std::string& name, const std::string& displayName);

        void RegisterTool(class ITool* tool);

        virtual void Render();
        
        virtual const std::string& GetName() const { return m_Name; }
        virtual const std::string& GetDisplayName() const { return m_DisplayName; };
        virtual const ImGuiWindowClass& GetToolWindowClass() const  { return m_ToolWindowClass; }
    
    protected:
        const std::string m_Name;
        const std::string m_DisplayName;
        ImGuiWindowClass m_ToolWindowClass;

        std::vector<class ITool*> m_Tools;
    };
}