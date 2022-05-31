#include "ActionSearchTool.h"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <array>

namespace AmaterasuDemo
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

		const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
		size_t matches = 0;
		for (int i = 0; i < std::size(lines); i++)
		{
			if (filter.PassFilter(lines[i]))
			{
				matches++;
				ImGui::MenuItem(lines[i], "CTRL+M");
			}
		}

		if (matches == 0)
		{
			ImGui::MenuItem("No matching commands");
		}

		ImGui::End();
	}
}