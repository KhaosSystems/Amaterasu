#include "WorkspaceStack.h"

#include <format>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
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
			ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_CentralNode;
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

			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags, window_class);

			ImGuiDockNode* dockNode = (ImGuiDockNode*)GImGui->DockContext.Nodes.GetVoidPtr(dockspace_id);

			if (ImGui::DockNodeBeginAmendTabBar(dockNode))
			{
				ImGui::TabItemButton("K", ImGuiTabItemFlags_Leading); // TODO: Make use icon font.
				ImGui::DockNodeEndAmendTabBar();
			}

			ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + ImVec2(ImGui::GetContentRegionAvail().x - 95.0f, 2.0f));
			if (ImGui::BeginTabBar("TheIdealSituation2"))
			{
				// TODO: Make use icon font.
				if (ImGui::TabItemButton("_", ImGuiTabItemFlags_Trailing));
				if (ImGui::TabItemButton("[]", ImGuiTabItemFlags_Trailing));
				if (ImGui::TabItemButton("X", ImGuiTabItemFlags_Trailing));
				ImGui::EndTabBar();
			}

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