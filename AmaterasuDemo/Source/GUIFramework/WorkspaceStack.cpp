#include "WorkspaceStack.h"

#include <format>

namespace AmaterasuDemo
{
    WorkspaceStack::WorkspaceStack()
    {
        m_WorkspaceWindowClass = ImGuiWindowClass();
        m_WorkspaceWindowClass.ClassId = ImHashStr("WorkspaceStackClass");
        m_WorkspaceWindowClass.DockingAllowUnclassed = false;
    }

    void WorkspaceStack::RegisterWorkspace(Workspace* workspace)
    {
        m_Workspaces.push_back(workspace);
    }

    void WorkspaceStack::Render()
    {
		//ImGui::ShowDemoWindow();
		ImGuiIO& io = ImGui::GetIO();
		ImGuiContext& g = *GImGui;
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiStyle oldStyle = style;

		// Workspaces
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		window_flags |= ImGuiWindowFlags_NoNavFocus;

		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.FramePadding = ImVec2(8.0f, 8.0f);
		style.WindowPadding = ImVec2(0.0f, 0.0f);

		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
			const ImGuiWindowClass* window_class = &m_WorkspaceWindowClass;

			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);

			ImGuiWindowFlags host_window_flags = 0;
			host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
			host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				host_window_flags |= ImGuiWindowFlags_NoBackground;

			char label[32];
			ImFormatString(label, IM_ARRAYSIZE(label), "DockSpaceViewport_%08X", viewport->ID);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin(label, NULL, host_window_flags);
			ImGui::PopStyleVar(3);

			ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.0f, 2.0f));

			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags, window_class);
			ImGui::End();
		}
		// ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_None, &workspaceClass);

        for (Workspace* workspace : m_Workspaces)
        {
            workspace->Render();
        }

        style = oldStyle;
    }
}