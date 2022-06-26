#include "NodeGraphDebugTool.h"

#include "NodeGraphDemoWorkspace.h"

#include <iostream>
#include <string>

namespace AmaterasuDemo
{
	NodeGraphDebugTool::NodeGraphDebugTool()
		: Tool<NodeGraphDemoWorkspace, NodeGraphDebugTool>("Node Graph Viewport Tool"), m_Workspace(nullptr)
	{
	}

	void NodeGraphDebugTool::Initialize(NodeGraphDemoWorkspace* workspace)
	{
		m_Workspace = workspace;
	}

	void NodeGraphDebugTool::Render()
	{
		ImGui::Text("Nodes [%i]", m_Workspace->MyNodeGraph.Nodes.size());
		for (INode* node : m_Workspace->MyNodeGraph.Nodes)
		{
			if (ImGui::TreeNode(node->GetIdentifier().ToString().c_str()))
			{
				ImGui::Text("Input parameters [%i]", node->GetInputParameters().size());
				for (IInputParameter* inputParameter : node->GetInputParameters())
				{
					// if (ImGui::TreeNode(&inputParameter)) { ImGui::TreePop(); }
				}

				ImGui::Text("Output parameters [%i]", node->GetOutputParameters().size());
				for (IOutputParameter* outputParameter : node->GetOutputParameters())
				{
					// if (ImGui::TreeNode(&outputParameter)) { ImGui::TreePop(); }
				}

				ImGui::TreePop();
			}
		}
	}
}