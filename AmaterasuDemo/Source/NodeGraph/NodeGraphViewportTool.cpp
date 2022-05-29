#include "NodeGraphViewportTool.h"

#include "NodeGraphDemoWorkspace.h"

namespace AmaterasuDemo
{
	NodeGraphViewportTool::NodeGraphViewportTool()
		: Tool<NodeGraphDemoWorkspace, NodeGraphViewportTool>("Node Graph Viewport Tool"), m_ViewportPosition(ImVec2(0.0f, 0.0f)), m_ViewportZoom(1.0f)
	{
	}

	void NodeGraphViewportTool::Initialize(NodeGraphDemoWorkspace* workspace)
	{
		m_Workspace = workspace;
	}

	void NodeGraphViewportTool::Render()
	{
		ImGui::ShowDemoWindow();

		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImVec2(ImGui::GetContentRegionAvail().x, std::max(ImGui::GetContentRegionAvail().y, 50.0f));
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		drawList->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(26, 26, 26, 255));

		// This will catch our interactions
		ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held

		// Pan
		bool opt_enable_context_menu = true;
		const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
		{
			m_ViewportPosition.x += io.MouseDelta.x;
			m_ViewportPosition.y += io.MouseDelta.y;
		}

		// Context menu (under default mouse threshold)
		ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
		if (opt_enable_context_menu && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
			ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

		if (ImGui::BeginPopup("context"))
		{
			if (ImGui::MenuItem("Zoom in", NULL, false, m_ViewportZoom < 2.0f)) { m_ViewportZoom += 0.25f; }
			if (ImGui::MenuItem("Zoom out", NULL, false, m_ViewportZoom > 0.25f)) { m_ViewportZoom -= 0.25f; }
			ImGui::EndPopup();
		}

		// Grid
		drawList->PushClipRect(canvas_p0, canvas_p1, true);
		const float GRID_STEP = 64.0f * m_ViewportZoom;
		for (float x = fmodf(m_ViewportPosition.x * m_ViewportZoom, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
		{
			drawList->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
		}
		for (float y = fmodf(m_ViewportPosition.y * m_ViewportZoom, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
		{
			drawList->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
		}

		const ImU32 nodeBackgroundColor = IM_COL32(59, 59, 59, 255);
		const ImU32 nodeBorderColor = IM_COL32(51, 51, 51, 255);
		const float nodeBorderRounding = 10.0f * m_ViewportZoom;
		const ImVec2 nodeBorderSize = ImVec2(2.0f, 2.0f) * m_ViewportZoom;
		const float nodeHeaderVerticalMargin = 13.0f * m_ViewportZoom;
		const float nodeFooterVerticalMargin = 13.0f * m_ViewportZoom;
		const float nodeDividerThickness = 1.0f * m_ViewportZoom;
		const float nodeParameterVerticalMargin = 18.0f * m_ViewportZoom;
		const float nodeParameterVerticalSpaceing = 22.0f * m_ViewportZoom;
		const float nodeParameterCircleRadius = 6.0f * m_ViewportZoom;
		const float nodeParameterBorderCircleRadius = 8.0f * m_ViewportZoom;

		for (INode* node : m_Workspace->MyNodeGraph.Nodes)
		{
			// Render node background
			ViewportNodeData* viewportNodeData = nullptr;

			std::map<INode*, ViewportNodeData>::iterator nodeViewportDataIterator = m_ViewportNodeData.find(node);
			if (nodeViewportDataIterator == m_ViewportNodeData.end())
			{
				ViewportNodeData newViewportNodeData;
				newViewportNodeData.Position = ImVec2(0.0f, 0.0f);
				m_ViewportNodeData[node] = newViewportNodeData;

				viewportNodeData = &m_ViewportNodeData[node];
			}
			else
			{
				viewportNodeData = &nodeViewportDataIterator->second;
			}

			const uint32_t nodeParameterCount = std::max(node->GetInputParameters().size(), node->GetOutputParameters().size());
			const float nodeParametersVerticalSize = (nodeParameterVerticalMargin * 2) + (nodeParameterVerticalSpaceing * (nodeParameterCount - 1));

			const ImVec2 nodePosition = (viewportNodeData->Position + m_ViewportPosition) * m_ViewportZoom + canvas_p0;
			const ImVec2 nodeSize = ImVec2(256.0f * m_ViewportZoom, nodeHeaderVerticalMargin + nodeParametersVerticalSize + nodeFooterVerticalMargin);

			// Background and border
			drawList->AddRectFilled(nodePosition, nodePosition + nodeSize, nodeBorderColor, nodeBorderRounding, ImDrawCornerFlags_All);
			drawList->AddRectFilled(nodePosition + nodeBorderSize, nodePosition + nodeSize - nodeBorderSize, nodeBackgroundColor, nodeBorderRounding, ImDrawCornerFlags_All);
			
			// Dividers
			drawList->AddRectFilled(nodePosition + ImVec2(0.0f, nodeHeaderVerticalMargin), nodePosition + ImVec2(nodeSize.x, nodeHeaderVerticalMargin + nodeDividerThickness), nodeBorderColor);
			drawList->AddRectFilled(nodePosition + ImVec2(0.0f, nodeSize.y - nodeHeaderVerticalMargin - nodeDividerThickness), nodePosition + ImVec2(nodeSize.x, nodeSize.y - nodeHeaderVerticalMargin), nodeBorderColor);

			// Name
			float displatNameFontSize = 20.0f * m_ViewportZoom;
			ImVec2 displayNameSize = ImGui::CalcTextSize("MyTitle") * (displatNameFontSize / io.FontDefault->FontSize);
			ImVec2 displayNamePosition = nodePosition + ImVec2((nodeSize.x - displayNameSize.x) * 0.5f, -displayNameSize.y * 1.5);
			drawList->AddText(io.FontDefault, displatNameFontSize, displayNamePosition, IM_COL32(194, 194, 194, 194), "MyTitle");

			// Input parameters
			ImVec2 parameterCursor = nodePosition + ImVec2(0.0f, nodeHeaderVerticalMargin + nodeParameterVerticalMargin);
			for (IInputParameter* inputParameter : node->GetInputParameters())
			{
				const ImU32 nodeParameterColor = IM_COL32(255, 255, 255, 255);

				drawList->AddCircleFilled(parameterCursor, nodeParameterBorderCircleRadius, nodeBorderColor);
				drawList->AddCircleFilled(parameterCursor, nodeParameterCircleRadius, nodeParameterColor);

				parameterCursor += ImVec2(0.0f, nodeParameterVerticalSpaceing);
			}

			// Output parameters
			parameterCursor = nodePosition + ImVec2(nodeSize.x, nodeHeaderVerticalMargin + nodeParameterVerticalMargin);
			for (IOutputParameter* outputParameter : node->GetOutputParameters())
			{
				const ImU32 nodeParameterColor = IM_COL32(255, 255, 255, 255);

				drawList->AddCircleFilled(parameterCursor, nodeParameterBorderCircleRadius, nodeBorderColor);
				drawList->AddCircleFilled(parameterCursor, nodeParameterCircleRadius, nodeParameterColor);

				parameterCursor += ImVec2(0.0f, nodeParameterVerticalSpaceing);
			}
		}
	}
}