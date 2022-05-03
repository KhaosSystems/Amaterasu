#include "NodeGraph.h"

#include <iostream>
#include <algorithm>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

// TODO: Only inputs save connection info? or output? who should save? Both might be handy.
// TODO: Make node data inspector (tree view).

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream> 
namespace AmaterasuDemo
{
	SceneNode::SceneNode(SceneNode* parent)
		: m_Parent(parent), m_Position(ImVec2(0.0f, 0.0f))
	{
	}

	void SceneNode::SetRelativePosition(ImVec2 newPosition)
	{
		m_Position = newPosition;
	}

	void SceneNode::SetWorldPosition(ImVec2 newPosition)
	{
		m_Position = m_Parent ? newPosition - m_Parent->GetWorldPosition() : newPosition;
	}

	ImVec2 SceneNode::GetRelativePosition()
	{
		return m_Position;
	}

	ImVec2 SceneNode::GetWorldPosition()
	{
		return m_Parent ? m_Parent->GetWorldPosition() + m_Position : m_Position;
	}

	bool SceneNode::IsOverlapping(ImVec2 point)
	{
		ImVec2 p1 = GetWorldPosition();
		ImVec2 p2 = GetWorldPosition() + ImVec2(100.0f, 100.0f);
		return point.x >= p1.x && point.x <= p2.x && point.y >= p1.y && point.y <= p2.y;
	}

	std::deque<SceneNode*>& SceneNode::GetChildren()
	{
		return m_Children;
	}

	void SceneNode::Render()
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		drawList->AddRectFilled(GetWorldPosition(), GetWorldPosition() + ImVec2(100.0f, 100.0f), IM_COL32(51, 51, 51, 255));
	}

	void Node::Serialize(rapidxml::xml_document<>& xmlDocument, rapidxml::xml_node<>* xmlParentNode)
	{
		rapidxml::xml_node<>* xmlNode = xmlDocument.allocate_node(rapidxml::node_type::node_element, "Node");

		// Serialize type name
		xmlNode->append_node(xmlDocument.allocate_node(rapidxml::node_type::node_element, "TypeName", GetTypeName().c_str()));

		// Serialize unique identifier
		xmlNode->append_node(xmlDocument.allocate_node(rapidxml::node_type::node_element, "UniqueIdentifier", m_UniqueIdentifier.c_str()));

		// Serialize display name
		xmlNode->append_node(xmlDocument.allocate_node(rapidxml::node_type::node_element, "DisplayName", GetDisplayName().c_str()));

		// Serialize position
		rapidxml::xml_node<>* xmlPositionNode = xmlDocument.allocate_node(rapidxml::node_type::node_element, "Position");
		xmlPositionNode->append_node(xmlDocument.allocate_node(rapidxml::node_type::node_element, "X", xmlDocument.allocate_string((std::ostringstream{} << std::fixed << std::setprecision(2) << (float)GetRelativePosition().x).str().c_str())));
		xmlPositionNode->append_node(xmlDocument.allocate_node(rapidxml::node_type::node_element, "Y", xmlDocument.allocate_string((std::ostringstream{} << std::fixed << std::setprecision(2) << (float)GetRelativePosition().y).str().c_str())));
		xmlNode->append_node(xmlPositionNode);

		// Serialize parameters
		rapidxml::xml_node<>* xmlParametersNode = xmlDocument.allocate_node(rapidxml::node_type::node_element, "Parameters");
		for (SceneNode* sceneNode : m_Children)
		{
			INodeParameter* parameter = dynamic_cast<INodeParameter*>(sceneNode);
			if (parameter)
			{
				parameter->Serialize(xmlDocument, xmlParametersNode);
			}
		}
		xmlNode->append_node(xmlParametersNode);

		xmlParentNode->append_node(xmlNode);
	}

	void Node::Deserialize(rapidxml::xml_node<>* xmlNode)
	{
		SetDisplayName(xmlNode->first_node("DisplayName")->value());
		m_UniqueIdentifier = xmlNode->first_node("UniqueIdentifier")->value();
		float positionX = std::stof(xmlNode->first_node("Position")->first_node("X")->value());
		float positionY = std::stof(xmlNode->first_node("Position")->first_node("Y")->value());
		SetRelativePosition(ImVec2(positionX, positionY));

		rapidxml::xml_node<>* xmlParametersNode = xmlNode->first_node("Parameters");
		for (rapidxml::xml_node<>* xmlNodeParameterNode = xmlParametersNode->first_node("Parameter"); xmlNodeParameterNode; xmlNodeParameterNode = xmlNodeParameterNode->next_sibling())
		{
			for (SceneNode* child : m_Children)
			{
				INodeParameter* nodeParameter = dynamic_cast<INodeParameter*>(child);
				if (nodeParameter && nodeParameter->GetDisplayName() == xmlNodeParameterNode->first_node("DisplayName")->value())
				{
					nodeParameter->Deserialize(xmlNodeParameterNode);
				}
			}
		}
	}

	void Node::DeserializeParameters(rapidxml::xml_node<>* xmlNode)
	{
		for (const auto& [key, value] : m_Inputs)
		{
			INodeParameter* parameter = dynamic_cast<INodeParameter*>(value);
			if (parameter)
			{
				rapidxml::xml_node<>* correspondingXmlNode = nullptr;
				rapidxml::xml_node<>* xmlParametersNode = xmlNode->first_node("Parameters");
				for (rapidxml::xml_node<>* xmlNodeParameterNode = xmlParametersNode->first_node("Parameter"); xmlNodeParameterNode; xmlNodeParameterNode = xmlNodeParameterNode->next_sibling())
				{
					if (xmlNodeParameterNode->first_node("UniqueIdentifier")->value() == parameter->GetUniqueIdentifier())
					{
						correspondingXmlNode = xmlNodeParameterNode;
						break;
					}
				}

				if (correspondingXmlNode)
				{
					parameter->DeserializeConnections(correspondingXmlNode);
				}
				else
				{
					std::cout << "Failed to find parameter" << std::endl;
				}
			}
		}
	}

	Node::Node(NodeGraph* parent)
		: SceneNode(parent)
	{
		m_Parent = parent;

		NodeGraph* graph = dynamic_cast<NodeGraph*>(m_Parent);
		assert(graph != nullptr);
		m_UniqueIdentifier = graph->GenerateUnqiueIdentifier();
	}

	void Node::Render()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		const float parameterVerticalMargin = 18.0f;
		const float parameterVerticalSpaceing = 22.0f;
		/*std::max(std::max(inputs.size() - 1.0f, outputs.size() - 1.0f), 0.0f)*/
		const float parameterVerticalSize = (parameterVerticalMargin * 2) + 5 * parameterVerticalSpaceing;

		const float nodeHeaderDividerThickness = 1.0f;

		const float nodeHeaderVerticalMargin = 13.0f;
		const float nodeFooterVerticalMargin = 13.0f;
		const ImVec2 nodeBorderSize = ImVec2(2.0f, 2.0f);
		const ImVec2 nodePosition = GetWorldPosition();
		const ImVec2 nodeSize = ImVec2(250.0f, nodeHeaderVerticalMargin + parameterVerticalSize + nodeFooterVerticalMargin);

		// Draw base
		float displatNameFontSize = 20.0f;
		ImVec2 displayNameSize = ImGui::CalcTextSize(GetDisplayName().c_str()) * (displatNameFontSize / io.FontDefault->FontSize);
		ImVec2 displayNamePosition = nodePosition + ImVec2((nodeSize.x - displayNameSize.x) * 0.5f, -displayNameSize.y * 1.5);
		drawList->AddText(io.FontDefault, displatNameFontSize, displayNamePosition, IM_COL32(194, 194, 194, 194), GetDisplayName().c_str());
		drawList->AddRectFilled(nodePosition, nodePosition + nodeSize, IM_COL32(51, 51, 51, 255), 10.0f, ImDrawCornerFlags_All);
		drawList->AddRectFilled(nodePosition + nodeBorderSize, nodePosition + nodeSize - nodeBorderSize, IM_COL32(59, 59, 59, 255), 10.0f, ImDrawCornerFlags_All);
		drawList->AddRectFilled(nodePosition + ImVec2(0.0f, nodeHeaderVerticalMargin), nodePosition + ImVec2(nodeSize.x, nodeHeaderVerticalMargin + nodeHeaderDividerThickness), IM_COL32(51, 51, 51, 255));
		drawList->AddRectFilled(nodePosition + ImVec2(0.0f, nodeSize.y - nodeHeaderVerticalMargin - nodeHeaderDividerThickness), nodePosition + ImVec2(nodeSize.x, nodeSize.y - nodeHeaderVerticalMargin), IM_COL32(51, 51, 51, 255));

		for (SceneNode* child : m_Children) {
			child->Render();
		}
	}

	bool Node::IsOverlapping(ImVec2 point)
	{
		ImVec2 p1 = GetWorldPosition() + ImVec2(-8.0f, -8.0f);
		ImVec2 p2 = GetWorldPosition() + ImVec2(250.0f, 200.0f) + ImVec2(8.0f, 8.0f);
		// ImDrawList* drawList = ImGui::GetWindowDrawList();
		// drawList->AddRectFilled(p1, p2, IM_COL32(51, 251, 51, 255));
		return point.x >= p1.x && point.x <= p2.x && point.y >= p1.y && point.y <= p2.y;;
	}

	NodeGraph::NodeGraph()
		: SceneNode(nullptr), m_StartNodeParameter(nullptr), m_DragItem(nullptr), m_IncrementalUniqueIdentifer(0)
	{
		RegisterNodeType<KSExecuteNode>();
		RegisterNodeType<KSAddFloatNode>();

		m_Children.push_back(new KSExecuteNode(this));
		m_Children.push_back(new KSAddFloatNode(this));
		m_Children.push_back(new KSAddFloatNode(this));
	}

	void NodeGraph::Initialize()
	{
	}

	void NodeGraph::Render()
	{
		ImGuiIO& io = ImGui::GetIO();

		m_LastMousePosition = m_MousePosition;
		m_LastMouseDown = m_MouseDown;
		m_LastMouseClicked = m_MouseClicked;
		m_LastMouseReleased = m_MouseReleased;
		m_MousePosition = ImGui::GetMousePos();
		m_MouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
		m_MouseClicked = ImGui::IsMouseDown(ImGuiMouseButton_Left) && !m_LastMouseClicked;
		m_MouseReleased = !ImGui::IsMouseDown(ImGuiMouseButton_Left) && m_LastMouseDown;

		ImGui::Begin("Node Graph Data");
		for (SceneNode* child : m_Children)
		{
			Node* node = dynamic_cast<Node*>(child);
			ImGui::Text("%s | %s", node->GetDisplayName().c_str(), node->GetUniqueIdentifier().c_str());
			for (const auto& [key, nodeInput] : node->GetInputsDictionary())
			{
				ImGui::Text(" - %s | %s", nodeInput->GetDisplayName().c_str(), nodeInput->GetUniqueIdentifier().c_str());
			}
			for (const auto& [key, nodeInput] : node->GetOutputsDictionary())
			{
				ImGui::Text(" - %s | %s", nodeInput->GetDisplayName().c_str(), nodeInput->GetUniqueIdentifier().c_str());
			}
		}
		ImGui::End();

		ImGui::Begin("Node Graph");

		m_Position = ImGui::GetCursorScreenPos();

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImVec2(ImGui::GetContentRegionAvail().x, std::max(ImGui::GetContentRegionAvail().y, 50.0f));
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(26, 26, 26, 255));
		//drawList->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

		ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

		drawList->PushClipRect(canvas_p0, canvas_p1, true);
		for (SceneNode* sceneNode : m_Children)
		{
			sceneNode->Render();
		}
		drawList->PopClipRect();


		if (io.MouseClicked[ImGuiMouseButton_Right])
		{
			ImGui::OpenPopup("NodeMenu");
		}

		if (ImGui::BeginPopupContextItem("NodeMenu", 3)) {
			if (ImGui::Selectable("Save"))
			{
				Serialize();
			}
			if (ImGui::Selectable("Load"))
			{
				Deserialize();
			}
			ImGui::EndPopup();
		}

		// Input
		for (SceneNode* child : m_Children)
		{
			if (io.MouseClicked[ImGuiMouseButton_Middle] && child->IsOverlapping(io.MousePos))
			{
				Node* node = dynamic_cast<Node*>(child);
				if (node)
				{
					node->Execute();
				}
			}

			if (m_MouseClicked && child->IsOverlapping(m_MousePosition))
			{
				for (SceneNode* childChild : child->GetChildren())
				{
					if (m_StartNodeParameter) continue;
					auto* nodeParameter = dynamic_cast<INodeParameter*>(childChild);
					if (nodeParameter && nodeParameter->IsOverlapping(m_MousePosition))
					{
						m_StartNodeParameter = nodeParameter;
					}
				}

				if (!m_StartNodeParameter)
				{
					m_DragItemOffset = child->GetWorldPosition() - m_MousePosition;
					m_DragItem = child;
				}
			}
		}

		if (m_MouseReleased)
		{
			for (SceneNode* child : m_Children)
			{
				if (!m_StartNodeParameter) break;

				if (child->IsOverlapping(m_MousePosition))
				{
					for (SceneNode* childChild : child->GetChildren())
					{
						if (!m_StartNodeParameter) break;

						auto* nodeParameter = dynamic_cast<INodeParameter*>(childChild);
						if (nodeParameter && nodeParameter->IsOverlapping(m_MousePosition))
						{
							m_StartNodeParameter->Connect(nodeParameter);
							m_StartNodeParameter = nullptr;
						}
					}
				}

			}
			m_DragItem = nullptr;
			m_StartNodeParameter = nullptr;
		}

		if (m_StartNodeParameter)
		{
			ImVec2 start = m_StartNodeParameter->GetWorldPosition();
			ImVec2 end = m_MousePosition;
			drawList->AddBezierCurve(start, ImVec2((end.x * 0.6) + (start.x * 0.4), start.y), ImVec2((end.x * 0.4) + (start.x * 0.6), end.y), end, IM_COL32(51, 51, 51, 255), 6);
			drawList->AddBezierCurve(start, ImVec2((end.x * 0.6) + (start.x * 0.4), start.y), ImVec2((end.x * 0.4) + (start.x * 0.6), end.y), end, IM_COL32(0, 194, 255, 255), 2);
		}

		if (m_DragItem)
		{
			m_DragItem->SetWorldPosition(m_MousePosition + m_DragItemOffset);
		}

		ImGui::End(); // Node Graph
	}

	void NodeGraph::Terminate()
	{
	}

	void NodeGraph::Serialize()
	{
		rapidxml::xml_document<> xmlDocument;
		rapidxml::xml_node<>* xmlRootNode = xmlDocument.allocate_node(rapidxml::node_type::node_element, "NodeGraph");

		rapidxml::xml_node<>* xmlNodesNode = xmlDocument.allocate_node(rapidxml::node_type::node_element, "Nodes");
		for (SceneNode* sceneNode : m_Children)
		{
			Node* node = dynamic_cast<Node*>(sceneNode);
			if (node)
			{
				node->Serialize(xmlDocument, xmlNodesNode);
			}
		}
		xmlRootNode->append_node(xmlNodesNode);

		xmlDocument.append_node(xmlRootNode);

		// Save to file.
		std::ofstream file = std::ofstream("project.xml");
		if (file.is_open())
		{
			file << xmlDocument;
			file.close();
		}
	}

	void NodeGraph::Deserialize()
	{
		m_Children.clear();

		// Load from file.
		std::ifstream file = std::ifstream("project.xml");
		std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		buffer.push_back('\0');

		rapidxml::xml_document<> xmlDocument;
		xmlDocument.parse<0>(&buffer[0]);

		rapidxml::xml_node<>* xmlRootNode = xmlDocument.first_node("NodeGraph");

		rapidxml::xml_node<>* xmNodesNode = xmlRootNode->first_node("Nodes");
		for (rapidxml::xml_node<>* xmlNode = xmNodesNode->first_node("Node"); xmlNode; xmlNode = xmlNode->next_sibling())
		{
			Node* newNode = m_NodeTypes[xmlNode->first_node("TypeName")->value()](this);
			newNode->Deserialize(xmlNode);
			m_Children.push_back(newNode);
		}

		for (SceneNode* sceneNode : m_Children)
		{
			Node* node = dynamic_cast<Node*>(sceneNode);
			if (node)
			{
				rapidxml::xml_node<>* correspondingXmlNode = nullptr;
				for (rapidxml::xml_node<>* xmlNode = xmNodesNode->first_node("Node"); xmlNode; xmlNode = xmlNode->next_sibling())
				{
					if (xmlNode->first_node("UniqueIdentifier")->value() == node->GetUniqueIdentifier())
					{
						correspondingXmlNode = xmlNode;
						break;
					}
				}

				if (correspondingXmlNode)
				{
					node->DeserializeParameters(correspondingXmlNode);
				}
			}
		}
	}

	// Nodes 
	KSAddFloatNode::KSAddFloatNode(NodeGraph* parent)
		: Node(parent)
	{
		SetTypeName("KSAdd");
		SetDisplayName("Add");

		RegisterInput<float>("A");
		RegisterInput<float>("B");
		RegisterOutput<float>("C");
	}

	void KSAddFloatNode::Execute()
	{
		std::cout << GetDisplayName() << '\n';
		SetOutput<float>("C", GetInput<float>("A") + GetInput<float>("B"));
	}

	// 
	KSExecuteNode::KSExecuteNode(NodeGraph* parent)
		: Node(parent)
	{
		SetTypeName("KSExecute");
		SetDisplayName("Execute");

		RegisterOutput<ExecuteInfo>("Execute");
	}

	void KSExecuteNode::Execute()
	{
		for (INodeParameter* connection : m_Outputs["Execute"]->GetConnections())
		{
			Node* node = dynamic_cast<Node*>(connection->GetParent());
			if (node)
			{
				node->Execute();
			}
		}
	}
}