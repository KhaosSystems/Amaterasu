#include "Amaterasu/Core/Application.h"

#include <iostream>

#include <imgui.h>
#include <imgui_node_editor.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <concepts>
#include <type_traits>

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <typeindex>

namespace ed = ax::NodeEditor;

namespace AmaterasuDemo
{

	class INodeParameter
	{
	public:
		virtual void GetData(void* data) = 0;
		virtual void SetData(void* data) = 0;
		virtual std::type_index GetDataType() const = 0;
		virtual uint32_t GetDataSize() const = 0;
	};

	template<typename T>
	class NodeParameter : public INodeParameter
	{
	public:
		virtual void GetData(void* data) override { memcpy(data, &m_Data, sizeof(T)); }
		virtual void SetData(void* data) override { memcpy(&m_Data, data, sizeof(T)); }
		virtual std::type_index GetDataType() const override { return std::type_index(typeid(T)); }
		virtual uint32_t GetDataSize() const override { return sizeof(T); }
	
	private:
		T m_Data;
	};

	class INode
	{
	public:
		virtual void Execute() = 0;

		template<typename T>
		void RegisterInput(const std::string key)
		{
			assert(!m_Inputs.contains(key));

			m_Inputs[key] = new NodeParameter<T>();
		}

		template<typename T>
		void RegisterOutput(const std::string key)
		{
			assert(!m_Output.contains(key));

			m_Output[key] = new NodeParameter<T>();
		}

		template<typename T>
		void SetOutput(const std::string key, T data)
		{
			assert(m_Output.contains(key));
			assert(m_Output[key]->GetDataType() == std::type_index(typeid(T)));

			m_Output[key]->SetData(&data);
		}

		template<typename T>
		T GetInput(const std::string key)
		{
			assert(m_Inputs.contains(key));
			assert(m_Inputs[key]->GetDataType() == std::type_index(typeid(T)));

			T data;
			m_Inputs[key]->GetData(&data);

			return data;
		}

		std::unordered_map<std::string, INodeParameter*>& GetInputsDictionary() { return m_Inputs; }
		std::unordered_map<std::string, INodeParameter*>& GetOutputsDictionary() { return m_Output; }


	protected:
		std::unordered_map<std::string, INodeParameter*> m_Inputs;
		std::unordered_map<std::string, INodeParameter*> m_Output;
	};

	class KSAddFloatNode : public INode
	{
	public:
		KSAddFloatNode()
		{
			RegisterInput<float>("A");
			RegisterInput<float>("B");
			RegisterOutput<float>("C");
		}

		virtual void Execute() override
		{
			SetOutput<float>("C", GetInput<float>("A") + GetInput<float>("B"));
		}
	};

	class NodeGraph2
	{
	public:
		struct NodeInfo
		{
			INode* Data;
			std::string Title;
			ImVec2 Position;
			ImVec2 Size;
			bool Active = false;
			bool Hovered = false;
		};
	
	public:
		NodeGraph2()
		{
			
			NodeInfo nodeInfo1;
			nodeInfo1.Data = new KSAddFloatNode();
			nodeInfo1.Title = "Add";
			nodeInfo1.Position = ImVec2(200.0f, 220.0f);
			nodeInfo1.Size = ImVec2(250.0f, 84.0f);
			m_Nodes.push_back(nodeInfo1);

			NodeInfo nodeInfo2;
			nodeInfo2.Data = new KSAddFloatNode();
			nodeInfo2.Title = "Add";
			nodeInfo2.Position = ImVec2(570.0f, 320.0f);
			nodeInfo2.Size = ImVec2(250.0f, 84.0f);
			m_Nodes.push_back(nodeInfo2);
		}

		void Initialize() {}

		void RenderNode(ImDrawList* drawList, ImVec2 offset, const NodeInfo& node)
		{
			ImGuiIO& io = ImGui::GetIO();

			std::unordered_map<std::string, INodeParameter*>& inputs = node.Data->GetInputsDictionary();
			std::unordered_map<std::string, INodeParameter*>& outputs = node.Data->GetOutputsDictionary();

			const uint32_t parameterVerticalMargin = 18.0f;
			const uint32_t parameterVerticalSpaceing = 22.0f;
			const uint32_t parameterVerticalSize = (parameterVerticalMargin * 2) + std::max(std::max(inputs.size() - 1.0f, outputs.size() - 1.0f), 0.0f) * parameterVerticalSpaceing;

			const uint32_t nodeHeaderDividerThickness = 1.0f;

			const uint32_t nodeHeaderVerticalMargin = 13.0f;
			const uint32_t nodeFooterVerticalMargin = 13.0f;
			const ImVec2 nodeBorderSize = ImVec2(2.0f, 2.0f);
			const ImVec2 nodePosition = offset + node.Position;
			const ImVec2 nodeSize = ImVec2(250.0f, nodeHeaderVerticalMargin + parameterVerticalSize + nodeFooterVerticalMargin);

			// Draw base
			drawList->AddText(io.FontDefault, 20.0f, nodePosition + ImVec2(0.0f, -34.0f), IM_COL32(194, 194, 194, 194), node.Title.c_str());
			drawList->AddRectFilled(nodePosition, nodePosition + nodeSize, IM_COL32(51, 51, 51, 255), 10.0f, ImDrawCornerFlags_All);
			drawList->AddRectFilled(nodePosition + nodeBorderSize, nodePosition + nodeSize - nodeBorderSize, IM_COL32(59, 59, 59, 255), 10.0f, ImDrawCornerFlags_All);
			drawList->AddRectFilled(nodePosition + ImVec2(0.0f, nodeHeaderVerticalMargin), nodePosition + ImVec2(node.Size.x, nodeHeaderVerticalMargin + nodeHeaderDividerThickness), IM_COL32(51, 51, 51, 255));
			drawList->AddRectFilled(nodePosition + ImVec2(0.0f, nodeSize.y - nodeHeaderVerticalMargin - nodeHeaderDividerThickness), nodePosition + ImVec2(nodeSize.x, nodeSize.y - nodeHeaderVerticalMargin), IM_COL32(51, 51, 51, 255));

			// Draw parameters
			ImVec2 cursor = ImVec2(0.0f, nodeHeaderVerticalMargin + parameterVerticalMargin);
			for (auto const& [key, nodeParameter] : inputs)
			{
				drawList->AddCircleFilled(nodePosition + cursor, 8, IM_COL32(51, 51, 51, 255));
				drawList->AddCircleFilled(nodePosition + cursor, 6, IM_COL32(0, 194, 255, 255));
				cursor.y += parameterVerticalSpaceing;
			}

			cursor = ImVec2(nodeSize.x, nodeHeaderVerticalMargin + parameterVerticalMargin);
			for (auto const& [key, nodeParameter] : outputs)
			{
				drawList->AddCircleFilled(nodePosition + cursor, 8, IM_COL32(51, 51, 51, 255));
				drawList->AddCircleFilled(nodePosition + cursor, 6, IM_COL32(0, 194, 255, 255));
				cursor.y += parameterVerticalSpaceing;
			}
		}

		NodeInfo* draggingNode = nullptr;
		ImVec2 scrolling = ImVec2(0.0f, 0.0f);

		ImVec2 lastMousePosition = ImVec2(0.0f, 0.0f);
		bool lastMouseDown = false;
		bool lastMouseClicked = false;
		bool lastMouseReleased = false;
		ImVec2 mousePosition = ImVec2(0.0f, 0.0f);
		bool mouseDown = false;
		bool mouseClicked = false;
		bool mouseReleased = false;

		void ImGuiRender()
		{
			// Begin dockspace
			{
				ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

				ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

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

			{
				ImGuiIO& io = ImGui::GetIO();

				ImGui::Begin("Node Graph");
				{
					lastMousePosition = mousePosition;
					lastMouseDown = mouseDown;
					lastMouseClicked = mouseClicked;
					lastMouseReleased = mouseReleased;
					mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
					mouseClicked = ImGui::IsMouseDown(ImGuiMouseButton_Left) && !lastMouseClicked;
					mouseReleased = !ImGui::IsMouseDown(ImGuiMouseButton_Left) && lastMouseDown;
					mousePosition = io.MousePos;

					ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
					ImVec2 canvas_sz = ImVec2(ImGui::GetContentRegionAvail().x, std::max(ImGui::GetContentRegionAvail().y, 50.0f));
					ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

					const ImVec2 origin((canvas_p0.x + scrolling.x, 50.0f), std::max(canvas_p0.y + scrolling.y, 50.0f)); // Lock scrolled origin

					ImDrawList* drawList = ImGui::GetWindowDrawList();
					drawList->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(26, 26, 26, 255));
					drawList->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

					ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
					const bool isHovered = ImGui::IsItemHovered();
					const bool isActive = ImGui::IsItemActive();

					for (NodeInfo& node : m_Nodes)
					{
						node.Hovered = io.MousePos.x >= origin.x + node.Position.x && io.MousePos.x <= origin.x + node.Position.x + node.Size.x && io.MousePos.y >= origin.y + node.Position.y && io.MousePos.y <= origin.y + node.Position.y + node.Size.y;
						draggingNode = mouseClicked && node.Hovered ? &node : (mouseReleased ? nullptr : draggingNode);
					}

					if (draggingNode)
					{
						// Getting the clicked position and subtracting that from the current mouse position might result in smoother dragging.
						draggingNode->Position += ImGui::GetMousePos() - lastMousePosition;
					}

					ImVec2 start = origin + m_Nodes[0].Position + (m_Nodes[0].Size * ImVec2(1.0f, 0.0f)) + ImVec2(0.0f, 13.0f + 18.0f);
					ImVec2 end = origin + m_Nodes[1].Position + ImVec2(0.0f, 13.0f + 18.0f);
					drawList->AddBezierCurve(start, ImVec2((end.x * 0.6) + (start.x * 0.4), start.y), ImVec2((end.x * 0.4) + (start.x * 0.6), end.y), end, IM_COL32(51, 51, 51, 255), 6);
					drawList->AddBezierCurve(start, ImVec2((end.x * 0.6) + (start.x * 0.4), start.y), ImVec2((end.x * 0.4) + (start.x * 0.6), end.y), end, IM_COL32(0, 194, 255, 255), 2);


					drawList->PushClipRect(canvas_p0, canvas_p1, true);
					for (const NodeInfo& node : m_Nodes)
					{
						RenderNode(drawList, origin, node);
					}
					drawList->PopClipRect();
				}
				ImGui::End(); // Node Graph
			}

			{
				ImGui::End(); // Dockspace
			}
		}
		void Terminate() {}

	private:
		std::vector<NodeInfo> m_Nodes;
	};

	class NodeGraph
	{
	private:
		struct LinkInfo
		{
			ed::LinkId Id;
			ed::PinId  InputId;
			ed::PinId  OutputId;
		};

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

			// Start interaction with editor.
			ed::Begin("My Editor", ImVec2(0.0, 0.0f));

			int uniqueId = 1;

			//
			// 1) Commit known data to editor
			//

			// Submit Node A
			ed::NodeId nodeA_Id = uniqueId++;
			ed::PinId  nodeA_InputPinId = uniqueId++;
			ed::PinId  nodeA_OutputPinId = uniqueId++;

			if (g_FirstFrame)
				ed::SetNodePosition(nodeA_Id, ImVec2(10, 10));
			ed::BeginNode(nodeA_Id);
			ImGui::Text("Node A");
			ed::BeginPin(nodeA_InputPinId, ed::PinKind::Input);
			ImGui::Text("-> In");
			ed::EndPin();
			ImGui::SameLine();
			ed::BeginPin(nodeA_OutputPinId, ed::PinKind::Output);
			ImGui::Text("Out ->");
			ed::EndPin();
			ed::EndNode();

			// Submit Node B
			ed::NodeId nodeB_Id = uniqueId++;
			ed::PinId  nodeB_InputPinId1 = uniqueId++;
			ed::PinId  nodeB_InputPinId2 = uniqueId++;
			ed::PinId  nodeB_OutputPinId = uniqueId++;

			if (g_FirstFrame)
				ed::SetNodePosition(nodeB_Id, ImVec2(210, 60));
			ed::BeginNode(nodeB_Id);
			ImGui::Text("Node B");
			ImGuiEx_BeginColumn();
			ed::BeginPin(nodeB_InputPinId1, ed::PinKind::Input);
			ImGui::Text("-> In1");
			ed::EndPin();
			ed::BeginPin(nodeB_InputPinId2, ed::PinKind::Input);
			ImGui::Text("-> In2");
			ed::EndPin();
			ImGuiEx_NextColumn();
			ed::BeginPin(nodeB_OutputPinId, ed::PinKind::Output);
			ImGui::Text("Out ->");
			ed::EndPin();
			ImGuiEx_EndColumn();
			ed::EndNode();

			// Submit Links
			for (auto& linkInfo : g_Links)
				ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);

			//
			// 2) Handle interactions
			//

			// Handle creation action, returns true if editor want to create new object (node or link)
			if (ed::BeginCreate())
			{
				ed::PinId inputPinId, outputPinId;
				if (ed::QueryNewLink(&inputPinId, &outputPinId))
				{
					// QueryNewLink returns true if editor want to create new link between pins.
					//
					// Link can be created only for two valid pins, it is up to you to
					// validate if connection make sense. Editor is happy to make any.
					//
					// Link always goes from input to output. User may choose to drag
					// link from output pin or input pin. This determine which pin ids
					// are valid and which are not:
					//   * input valid, output invalid - user started to drag new ling from input pin
					//   * input invalid, output valid - user started to drag new ling from output pin
					//   * input valid, output valid   - user dragged link over other pin, can be validated

					if (inputPinId && outputPinId) // both are valid, let's accept link
					{
						// ed::AcceptNewItem() return true when user release mouse button.
						if (ed::AcceptNewItem())
						{
							// Since we accepted new link, lets add one to our list of links.
							g_Links.push_back({ ed::LinkId(g_NextLinkId++), inputPinId, outputPinId });

							// Draw new link.
							ed::Link(g_Links.back().Id, g_Links.back().InputId, g_Links.back().OutputId);
						}

						// You may choose to reject connection between these nodes
						// by calling ed::RejectNewItem(). This will allow editor to give
						// visual feedback by changing link thickness and color.
					}
				}
			}
			ed::EndCreate(); // Wraps up object creation action handling.


			// Handle deletion action
			if (ed::BeginDelete())
			{
				// There may be many links marked for deletion, let's loop over them.
				ed::LinkId deletedLinkId;
				while (ed::QueryDeletedLink(&deletedLinkId))
				{
					// If you agree that link can be deleted, accept deletion.
					if (ed::AcceptDeletedItem())
					{
						// Then remove link from your data.
						for (auto& link : g_Links)
						{
							if (link.Id == deletedLinkId)
							{
								g_Links.erase(&link);
								break;
							}
						}
					}

					// You may reject link deletion by calling:
					// ed::RejectDeletedItem();
				}
			}
			ed::EndDelete(); // Wrap up deletion action



			// End of interaction with editor.
			ed::End();

			if (g_FirstFrame)
				ed::NavigateToContent(0.0f);

			ed::SetCurrentEditor(nullptr);

			g_FirstFrame = false;

			// ImGui::ShowMetricsWindow();
		}

		void Terminate()
		{
			ed::DestroyEditor(g_Context);

		}

	private:
		ed::EditorContext* g_Context = nullptr;    // Editor context, required to trace a editor state.
		bool                g_FirstFrame = true;    // Flag set for first frame only, some action need to be executed once.
		ImVector<LinkInfo>  g_Links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
		int                 g_NextLinkId = 100;     // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.

	};

	class AmaterasuDemoApplication : public Amaterasu::Application
	{
	public:
		AmaterasuDemoApplication();
		~AmaterasuDemoApplication();

		void ImGuiRender() override;

	private:
		NodeGraph2 m_NodeGraph;
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

		ImGui::ShowDemoWindow();
	}
}

int main(int argc, char* argv[])
{
	AmaterasuDemo::AmaterasuDemoApplication application = AmaterasuDemo::AmaterasuDemoApplication();
	application.Run();
}