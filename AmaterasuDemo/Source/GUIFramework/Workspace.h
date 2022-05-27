#pragma once

#include "Tool.h"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <string>
#include <vector>
#include <typeinfo>
#include <iostream>
#include <assert.h>

namespace AmaterasuDemo
{
	class IWorkspace
	{
	public:
		virtual void Render() = 0;
	};

	template<typename WorkspaceType>
	class Workspace : public IWorkspace
	{
	public:
		Workspace(const std::string& displayName)
			: m_Name(typeid(WorkspaceType).name()), m_DisplayName(displayName)
		{
			m_ToolWindowClass = ImGuiWindowClass();
			m_ToolWindowClass.ClassId = ImHashStr(std::string(m_DisplayName + "###WindowClass_" + m_Name).c_str());
			m_ToolWindowClass.DockingAllowUnclassed = false;
		}

		template<std::derived_from<ITool> ToolType>
		void RegisterTool()
		{
			// WARN: It's 03:54... This code is pretty dangerous, better check it when I an not sleep deprived...
			static_assert(std::is_base_of<ITool, ToolType>::value);

			std::unique_ptr<ToolType> newTool = std::make_unique<ToolType>();
			assert(newTool->GetWorkspaceTypeInfo() == typeid(WorkspaceType));
			m_Tools.push_back(std::unique_ptr<ITool>{static_cast<ITool*>(newTool.release())});
		}

		virtual void Render() override
		{
			ImGuiStyle& style = ImGui::GetStyle();
			ImGuiStyle oldStyle = style;

			ImGuiWindowClass workspaceWindowClass{};
			workspaceWindowClass.ClassId = ImHashStr("WorkspaceStackClass");
			workspaceWindowClass.DockingAllowUnclassed = false;

			style.WindowMenuButtonPosition = ImGuiDir_Left;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 8.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowClass(&workspaceWindowClass);
			std::string windowName = m_DisplayName + "###WorkspaceWindow_" + m_Name;
			ImGui::Begin(windowName.c_str());
			ImGui::PopStyleVar(2);

			ImDrawList* drawList = ImGui::GetWindowDrawList();
			ImVec2 p1 = ImGui::GetCursorScreenPos();
			ImVec2 p2 = p1 + ImVec2(ImGui::GetContentRegionAvail().x + (style.WindowPadding.x * 2), 3.0f);
			drawList->AddRectFilled(p1, p2, IM_COL32(36, 36, 36, 255));
			ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.0f, 3.0f));

			style.WindowMenuButtonPosition = ImGuiDir_None;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
			ImGui::DockSpace(ImGui::GetID(windowName.c_str()), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None, &m_ToolWindowClass);
			ImGui::PopStyleVar(1);

			for (std::unique_ptr<ITool>& tool : m_Tools)
			{
				tool->Render();
			}

			ImGui::End();

			style = oldStyle;
		}

	protected:
		const std::string m_Name;
		const std::string m_DisplayName;
		ImGuiWindowClass m_ToolWindowClass;

		std::vector<std::unique_ptr<ITool>> m_Tools;
	};

}