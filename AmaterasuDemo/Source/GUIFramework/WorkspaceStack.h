#pragma once

#include "Workspace.h"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <string>
#include <vector>

namespace AmaterasuDemo
{
    class WorkspaceStack
    {
    public:
        WorkspaceStack();

        void Render();

        void RegisterWorkspace(Workspace* workspace);
        
        const ImGuiWindowClass& GetWorkspaceWindowClass() const  { return m_WorkspaceWindowClass; }

    private:
        std::vector<Workspace*> m_Workspaces;

        ImGuiWindowClass m_WorkspaceWindowClass;
    };
}