#include "Amaterasu/Core/Application.h"

#include <iostream>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <concepts>
#include <type_traits>

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <iostream>

#include "NodeGraph/NodeGraph.h"

#include "GUIFramework/Workspace.h"

namespace AmaterasuDemo
{
	class AmaterasuDemoApplication : public Amaterasu::Application
	{
	public:
		AmaterasuDemoApplication();
		~AmaterasuDemoApplication();

		void ImGuiRender() override;

	private:
		NodeGraph m_NodeGraph;

		Workspace m_Workspace1;
		Workspace m_Workspace2;
	};

	AmaterasuDemoApplication::AmaterasuDemoApplication()
		: Amaterasu::Application("Amaterasu Demo"), m_Workspace1(Workspace("TestWorkspace1")), m_Workspace2(Workspace("TestWorkspace2"))
	{
		m_NodeGraph.Initialize();
		std::cout << "called2" << std::endl;
	}

	AmaterasuDemoApplication::~AmaterasuDemoApplication()
	{
		m_NodeGraph.Terminate();
	}

	ImVec2 windowMoveOffset = ImVec2(0.0f, 0.0f);
	bool isDraggingWindow = false;
	void AmaterasuDemoApplication::ImGuiRender()
	{
		Application::ImGuiRender();

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

		ImGuiWindowClass workspaceClass;
		workspaceClass.ClassId = ImHashStr("Workspace");
		workspaceClass.DockingAllowUnclassed = false;

		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.FramePadding = ImVec2(8.0f, 8.0f);
		style.WindowPadding = ImVec2(0.0f, 0.0f);

		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
			const ImGuiWindowClass* window_class = &workspaceClass;

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

		{
			int windowX, windowY = 0;
			glfwGetWindowPos(GetWindow(), &windowX, &windowY);
			ImVec2 windowPos = ImVec2(windowX, windowY);
			ImVec2 p1 = ImVec2(300.0f, 0.0f) + windowPos;
			ImVec2 p2 = ImVec2(600.0f, 60.0f) + windowPos;
			ImVec2 point = io.MousePos;

			if (ImGui::IsMouseClicked(0))
			{
				if (point.x >= p1.x && point.x <= p2.x && point.y >= p1.y && point.y <= p2.y)
				{
					windowMoveOffset = windowPos - point;
					isDraggingWindow = true;
				}
			}

			if (ImGui::IsMouseReleased(0))
			{
				isDraggingWindow = false;
			}

			if (isDraggingWindow)
			{
				glfwSetWindowPos(GetWindow(), point.x + windowMoveOffset.x, point.y + windowMoveOffset.y);
			}
		}

		// Scene Editor Workspace
		m_Workspace1.Render();
		m_Workspace2.Render();

		style = oldStyle;

		return;

		// Begin dockspace
		{
			ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
			window_flags |= ImGuiWindowFlags_NoTitleBar;
			window_flags |= ImGuiWindowFlags_NoTitleBar;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoDocking;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
			window_flags |= ImGuiWindowFlags_NoNavFocus;

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace", (bool*)0, window_flags);
			ImGui::PopStyleVar(3);

			ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		int windowX, windowY = 0;
		glfwGetWindowPos(GetWindow(), &windowX, &windowY);
		ImVec2 windowPos = ImVec2(windowX, windowY);
		ImVec2 p1 = ImVec2(300.0f, 0.0f) + windowPos;
		ImVec2 p2 = ImVec2(600.0f, 60.0f) + windowPos;
		ImVec2 point = io.MousePos;
		drawList->AddRectFilled(p1, p2, IM_COL32(255, 100, 100, 255));

		if (ImGui::IsMouseClicked(0))
		{
			if (point.x >= p1.x && point.x <= p2.x && point.y >= p1.y && point.y <= p2.y)
			{
				windowMoveOffset = windowPos - point;
				isDraggingWindow = true;
			}
		}

		if (ImGui::IsMouseReleased(0))
		{
			isDraggingWindow = false;
		}

		if (isDraggingWindow)
		{
			glfwSetWindowPos(GetWindow(), point.x + windowMoveOffset.x, point.y + windowMoveOffset.y);
		}


		m_NodeGraph.Render();

		{
			ImGui::End(); // Dockspace
		}
	}
}

int main(int argc, char* argv[])
{
	AmaterasuDemo::AmaterasuDemoApplication application = AmaterasuDemo::AmaterasuDemoApplication();
	application.Run();
}