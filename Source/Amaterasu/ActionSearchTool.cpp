#include "ActionSearchTool.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <array>
#include <iostream>

namespace Amaterasu
{
	void ActionSearchTool::Render()
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		float nextWindowWidth = std::min(viewport->Size.x * 0.6f, 600.0f);
		ImGui::SetNextWindowSize(ImVec2(nextWindowWidth, 0.0f));
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x / 2, viewport->Pos.y + 15), 0, ImVec2(0.5f, 0.0f));

		ImGui::Begin("Action Seach", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

		if (ImGui::IsWindowAppearing()) {
			ImGui::SetKeyboardFocusHere(0);
		}

		static ImGuiTextFilter filter;
		filter.Draw();

		ImGui::Separator();

		size_t matches = 0;
		for (int i = 0; i < std::size(m_Actions); i++)
		{
			if (filter.PassFilter(m_Actions[i].name.c_str()))
			{
				matches++;
				if (ImGui::MenuItem(m_Actions[i].name.c_str(), "CTRL+M"))
				{
					m_Actions[i].Execute();
				}
			}
		}

		if (matches == 0)
		{
			ImGui::MenuItem("No matching commands");
		}

		ImGui::End();
	}

	void ActionSearchTool::AppendAction(Action action)
	{
		m_Actions.push_back(action);
	}
}