#pragma once

#include "Workspace.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <string>
#include <vector>

namespace Amaterasu
{
    class WorkspaceStack
    {
    public:
        WorkspaceStack();

        void Render();

        void RegisterWorkspace(IWorkspace* workspace);
        
        const ImGuiWindowClass& GetWorkspaceWindowClass() const  { return m_WorkspaceWindowClass; }

    private:
        std::vector<IWorkspace*> m_Workspaces;

        ImGuiWindowClass m_WorkspaceWindowClass;
    };
}