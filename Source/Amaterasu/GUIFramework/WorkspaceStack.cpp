#include "WorkspaceStack.h"

#include <format>

#include <imgui.h>
#include <imgui_internal.h>

namespace Amaterasu
{
	WorkspaceStack::WorkspaceStack()
	{
		m_WorkspaceWindowClass = ImGuiWindowClass();
		m_WorkspaceWindowClass.ClassId = ImHashStr("WorkspaceStackClass");
		m_WorkspaceWindowClass.DockingAllowUnclassed = false;
	}

	void WorkspaceStack::RegisterWorkspace(IWorkspace* workspace)
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
			ImGui::Begin(label, nullptr, host_window_flags);
			ImGui::PopStyleVar(3);

			// Title bar
			ImVec2 titleBarPos = ImGui::GetCursorScreenPos() + ImVec2(0.0f, 2.0f);
			ImGui::SetCursorScreenPos(titleBarPos);
			if (ImGui::BeginTabBar("TheIdealSituation"))
			{
				ImGui::TabItemButton("K", ImGuiTabItemFlags_Leading); // TODO: Make use icon font.
				ImGui::TabItemButton("Workspace 1");
				ImGui::TabItemButton("Workspace 2");
				ImGui::TabItemButton("Workspace 3");

				ImGui::EndTabBar();
			}
			
			ImGui::SetCursorScreenPos(titleBarPos + ImVec2(ImGui::GetContentRegionAvail().x - 96.0f, 0));
			if (ImGui::BeginTabBar("TheIdealSituation2"))
			{
				ImGui::TabItemButton("_", ImGuiTabItemFlags_Trailing); // TODO: Make use icon font.
				ImGui::TabItemButton("[]", ImGuiTabItemFlags_Trailing); // TODO: Make use icon font.
				ImGui::TabItemButton("X", ImGuiTabItemFlags_Trailing); // TODO: Make use icon font.
				ImGui::EndTabBar();
			}

			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags, window_class);

			// Title bar
			/*ImGuiDockNode* node = (ImGuiDockNode*)GImGui->DockContext.Nodes.GetVoidPtr(dockspace_id);
			if (node && node->TabBar)
			{
				// TODO: Make use icon font.
				ImGui::TabItemEx(node->TabBar, "K", nullptr, ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_Button | ImGuiTabItemFlags_NoReorder, nullptr);
				ImGui::TabItemEx(node->TabBar, "+", nullptr, ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_Button | ImGuiTabItemFlags_NoReorder, nullptr);
			}*/

			ImGui::End();

		}
		// ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_None, &workspaceClass);

		for (IWorkspace* workspace : m_Workspaces)
		{
			workspace->Render();
		}

		style = oldStyle;
	}
}