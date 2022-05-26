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

#include "GUIFramework/WorkspaceStack.h"
#include "NodeGraph/NodeGraphDemoWorkspace.h"

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

		WorkspaceStack m_WorkspaceStack;
		NodeGraphDemoWorkspace m_NodeGraphDemoWorkspace;
	};

	AmaterasuDemoApplication::AmaterasuDemoApplication()
		: Amaterasu::Application("Amaterasu Demo"), m_WorkspaceStack(), m_NodeGraphDemoWorkspace()
	{
		m_WorkspaceStack.RegisterWorkspace(&m_NodeGraphDemoWorkspace);

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

		ImGuiIO& io = ImGui::GetIO();

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

		m_WorkspaceStack.Render();
	}
}

int main(int argc, char* argv[])
{
	AmaterasuDemo::AmaterasuDemoApplication application = AmaterasuDemo::AmaterasuDemoApplication();
	application.Run();
}