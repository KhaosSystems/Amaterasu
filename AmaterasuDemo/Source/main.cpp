#include "Amaterasu/Core/Application.h"

#include <iostream>

#include <imgui.h>
#include <imgui_node_editor.h>
# define IMGUI_DEFINE_MATH_OPERATORS
# include <imgui_internal.h>

namespace ed = ax::NodeEditor;



namespace AmaterasuDemo
{
	void ImGuiEx_BeginColumn()
	{
		ImGui::BeginGroup();
	}

	void ImGuiEx_NextColumn()
	{
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
	}

	void ImGuiEx_EndColumn()
	{
		ImGui::EndGroup();
	}

	class NodeGraph
	{
	public:
		void Initialize()
		{
			ed::Config config;
			config.SettingsFile = "Simple.json";
			g_Context = ed::CreateEditor(&config);
		}
		
		void ImGuiRender()
		{
			auto& io = ImGui::GetIO();

			ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

			ImGui::Separator();

			ed::SetCurrentEditor(g_Context);
			ed::Begin("My Editor", ImVec2(0.0, 0.0f));
			int uniqueId = 1;
			// Start drawing nodes.
			ed::BeginNode(uniqueId++);
			ImGui::Text("Node A");
			ed::BeginPin(uniqueId++, ed::PinKind::Input);
			ImGui::Text("-> In");
			ed::EndPin();
			ImGui::SameLine();
			ed::BeginPin(uniqueId++, ed::PinKind::Output);
			ImGui::Text("Out ->");
			ed::EndPin();
			ed::EndNode();
			ed::End();
			ed::SetCurrentEditor(nullptr);
		}

		void Terminate()
		{
			ed::DestroyEditor(g_Context);

		}

	private:
		ed::EditorContext* g_Context = nullptr;
	};

	class AmaterasuDemoApplication : public Amaterasu::Application
	{
	public:
		AmaterasuDemoApplication();
		~AmaterasuDemoApplication();

		void ImGuiRender() override;

	private:
		NodeGraph m_NodeGraph;
	};

	AmaterasuDemoApplication::AmaterasuDemoApplication()
		: Amaterasu::Application("Amaterasu Demo")
	{
		m_NodeGraph.Initialize();
		std::cout << "called2" << std::endl;
	}

	AmaterasuDemoApplication::~AmaterasuDemoApplication()
	{
		m_NodeGraph.Terminate();
	}

	void AmaterasuDemoApplication::ImGuiRender()
	{
		Application::ImGuiRender();

		m_NodeGraph.ImGuiRender();
	}
}

int main(int argc, char* argv[])
{
	AmaterasuDemo::AmaterasuDemoApplication application = AmaterasuDemo::AmaterasuDemoApplication();
	application.Run();
}