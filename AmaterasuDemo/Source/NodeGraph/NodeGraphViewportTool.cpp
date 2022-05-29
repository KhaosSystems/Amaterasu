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
			if (ImGui::MenuItem("Zoom in", NULL, false, m_ViewportZoom > 0.5f)) { m_ViewportZoom -= 0.5f; }
			if (ImGui::MenuItem("Zoom out", NULL, false, m_ViewportZoom < 10.0f)) { m_ViewportZoom += 0.5f; }
			ImGui::EndPopup();
		}

		// Grid
		drawList->PushClipRect(canvas_p0, canvas_p1, true);
		const float GRID_STEP = 64.0f / m_ViewportZoom;
		for (float x = fmodf(m_ViewportPosition.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
		{
			drawList->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
		}
		for (float y = fmodf(m_ViewportPosition.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
		{
			drawList->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
		}

		for (INode* node : m_Workspace->MyNodeGraph.Nodes)
		{
			// Render node background

			for (IInputParameter* inputParameter : node->GetInputParameters())
			{
				// Render input parameter
			}

			for (IOutputParameter* outputParameter : node->GetOutputParameters())
			{
				// Render output parameter
			}
		}
	}
}