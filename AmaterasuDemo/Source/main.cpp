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

namespace ed = ax::NodeEditor;

namespace AmaterasuDemo
{
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r0.pdf
	template <class T, template<class...> class Primary> struct is_specialization_of : std::false_type {};
	template <template<class...> class Primary, class... Args> struct is_specialization_of<Primary<Args...>, Primary> : std::true_type {};
	template <class T, template<class...> class Primary> inline constexpr bool is_specialization_of_v = is_specialization_of<T, Primary>::value;

	template<typename... Ts> class NodeInputDefinition {};
	template<typename... Ts> class NoteOutputDefinition {};

	template <class T> concept IsNodeInputDefinition = is_specialization_of_v<T, NodeInputDefinition>;
	template <class T> concept IsNoteOutputDefinition = is_specialization_of_v<T, NoteOutputDefinition>;

	class INode
	{
	};

	template<IsNodeInputDefinition T1, IsNoteOutputDefinition T2>
	class Node : public INode
	{
	public:
		typedef T1 InputDefinition;
		typedef T2 OutputDefinition;
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
			NodeInfo nodeInfo;
			nodeInfo.Data = new Node<NodeInputDefinition<float, float>, NoteOutputDefinition<float>>();
			nodeInfo.Title = "Add";
			nodeInfo.Position = ImVec2(20.0f, 20.0f);
			nodeInfo.Size = ImVec2(250.0f, 100.0f);

			m_Nodes.push_back(nodeInfo);
		}

		void Initialize() {}


		void RenderNode(ImDrawList* drawList, ImVec2 offset, const NodeInfo& node)
		{
			ImGuiIO& io = ImGui::GetIO();

			drawList->AddText(io.FontDefault, 20.0f, offset + node.Position + ImVec2(0.0f, -34.0f), IM_COL32(194, 194, 194, 194), node.Title.c_str());
			drawList->AddRectFilled(offset + node.Position, offset + node.Position + node.Size, IM_COL32(51, 51, 51, 255), 10.0f, ImDrawCornerFlags_All);
			drawList->AddRectFilled(offset + node.Position + ImVec2(2.0f, 2.0f), offset + node.Position + node.Size - ImVec2(2.0f, 2.0f), IM_COL32(59, 59, 59, 255), 10.0f, ImDrawCornerFlags_All);
			drawList->AddRectFilled(offset + node.Position + ImVec2(0.0f, 13.0f), offset + node.Position + ImVec2(0.0f, 14.0f) + (node.Size * ImVec2(1.0f, 0.0f)), IM_COL32(51, 51, 51, 255));
			drawList->AddRectFilled(offset + node.Position + (node.Size * ImVec2(0.0f, 1.0f)) + ImVec2(0.0f, -14.0f), offset + node.Position + (node.Size * ImVec2(1.0f, 1.0f)) + ImVec2(0.0f, -13.0f), IM_COL32(51, 51, 51, 255));
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
			ImGuiIO& io = ImGui::GetIO();

			lastMousePosition = mousePosition;
			lastMouseDown = mouseDown;
			lastMouseClicked = mouseClicked;
			lastMouseReleased = mouseReleased;
			mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
			mouseClicked = ImGui::IsMouseDown(ImGuiMouseButton_Left) && !lastMouseClicked;
			mouseReleased = !ImGui::IsMouseDown(ImGuiMouseButton_Left) && lastMouseDown;
			mousePosition = io.MousePos;

			ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
			ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
			ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

			const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin

			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(26, 26, 26, 255));
			drawList->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

			ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
			const bool isHovered = ImGui::IsItemHovered();
			const bool isActive = ImGui::IsItemActive();

			for (NodeInfo& node : m_Nodes)
			{
				node.Hovered = io.MousePos.x >= origin.x + node.Position.x && io.MousePos.x <= origin.x + node.Position.x + node.Size.x && io.MousePos.y >= origin.y + node.Position.y && io.MousePos.y <= origin.y + node.Position.y + node.Size.y;
				draggingNode = mouseClicked ? &node : (mouseReleased ? nullptr : draggingNode);
			}

			if (draggingNode)
			{
				// Getting the clicked position and subtracting that from the current mouse position might result in smoother dragging.
				draggingNode->Position += ImGui::GetMousePos() - lastMousePosition;
			}

			drawList->PushClipRect(canvas_p0, canvas_p1, true);
			for (const NodeInfo& node : m_Nodes)
			{
				RenderNode(drawList, origin, node);
			}
			drawList->PopClipRect();
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