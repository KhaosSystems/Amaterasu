#pragma once

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <concepts>
#include <string>

namespace AmaterasuDemo
{
    class ITool
	{
    public: 
        ITool(const std::string& name, const std::string& displayName, const std::string& workspaceTypeName);

        virtual void Render();

    protected:
        
        const std::string m_Name;
        const std::string m_DisplayName;
        const std::string m_WorkspaceTypeName;
	};
}