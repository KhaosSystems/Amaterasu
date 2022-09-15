#include "WorkspaceStack.h"

#include <format>
#include <array>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "Amaterasu/Core/FileDialogs.h"

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

	char searchBuffer[256];

	bool fileWindowOpen = false;
	ImVec2 fileWindowPos = ImVec2(0.0f, 0.0f);
	int tick = 0;
	bool showDemoWindow = false;

	void WorkspaceStack::Render()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiContext& g = *GImGui;
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiStyle oldStyle = style;
		
		if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);

		if (fileWindowOpen)
		{
			ImGui::SetNextWindowPos(fileWindowPos);
			ImGui::SetNextWindowSize(ImVec2(300.0f, 400.0f));
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration;
			if (ImGui::Begin("stuff123457", &fileWindowOpen, flags))
			{
				ImDrawList* draw_list = ImGui::GetWindowDrawList();

				ImVec2 inputSize = ImVec2(0.0f, ImGui::CalcTextSize(searchBuffer).y + style.FramePadding.y * 2.0f);

				ImVec2 cursorPosWithWindowPadding = ImGui::GetCursorPos(); // TODO: Use some sort of custom style thing to access window padding directly
				ImVec2 rectSize = ImVec2(ImGui::GetWindowSize().x, inputSize.y + 10.0f);
				draw_list->AddRectFilled(ImVec2(0.0f, 0.0f), rectSize, ImColor(36, 36, 36, 255));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImU32)ImColor(21, 21, 21, 255));
				ImGui::SetCursorPos(ImVec2(5.0f, 5.0f));
				if (ImGui::InputTextEx("##consoleInput", "Enter Console Command", searchBuffer, IM_ARRAYSIZE(searchBuffer), inputSize,
					ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, nullptr, (void*)this))
				{
				}
				ImGui::PopStyleColor();

				ImGui::SetCursorPos(ImVec2(cursorPosWithWindowPadding.x, rectSize.y + 6.0f));

				if (ImGui::MenuItem("Open Folder", "Ctrl+Shift+O"))
				{
					fileWindowOpen = false;

					std::string filepath = FileDialogs::OpenFile("Any File");
				}

				if (ImGui::MenuItem("Show Demo Window"))
				{
					showDemoWindow = !showDemoWindow;
				}
			}
			ImGui::End();
		}

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
			
			// TODO: Drag on #TITLEBAR (See: https://github.com/ocornut/imgui/issues/5250)

			// TODO: Disable ImGuiWindowFlags_NoDocking on the window, if only one is in 

			if (ImGui::DockNodeBeginAmendTabBar(dockNode))
			{
				if (ImGui::TabItemButton("K", ImGuiTabItemFlags_Leading))// TODO: Use icon font.
				{
					fileWindowOpen = !fileWindowOpen;
					fileWindowPos = ImGui::GetCursorScreenPos();
				}

				// TODO: This is messy...
				float w = ImGui::TabItemCalcSize("_", false).x + ImGui::TabItemCalcSize("[]", false).x + ImGui::TabItemCalcSize("X", false).x;
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - w);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.0f);

				ImGui::TabItemButton("Spacer");

				ImGui::PopStyleVar();

				if (ImGui::TabItemButton("_", ImGuiTabItemFlags_Trailing))
				{
				}

				if (ImGui::TabItemButton("[]", ImGuiTabItemFlags_Trailing))
				{
				}

				if (ImGui::TabItemButton("X", ImGuiTabItemFlags_Trailing))
				{
					std::exit(0);
				}

				ImGui::DockNodeEndAmendTabBar();
			}

			ImGui::End();
		}
		// ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_None, &workspaceClass);

		for (IWorkspace* workspace : m_Workspaces)
		{
			workspace->Render();
		}

		style = oldStyle;

		// This window is basically just using 6.0f for padding - might be better than 8.0f..
	}
}