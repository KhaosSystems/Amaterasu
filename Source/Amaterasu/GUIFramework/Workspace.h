#pragma once

#include "Tool.h"
#include "Amaterasu/ActionSearchTool.h"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <string>
#include <vector>
#include <typeinfo>
#include <iostream>
#include <assert.h>

namespace Amaterasu
{
	class IWorkspace
	{
	public:
		virtual void Render() = 0;

		virtual const std::vector<Action*>& GetActions() const = 0;
		virtual const std::vector<ITool*>& GetTools() const = 0;
	};

	template<typename WorkspaceType>
	class Workspace : public IWorkspace
	{
	public:
		Workspace(const std::string& displayName)
			: m_Name(typeid(WorkspaceType).name()), m_DisplayName(displayName), m_ActionSearchTool(), m_ShowActionSearchTool(false)
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

			ToolType* newTool = new ToolType();
			assert(newTool->GetWorkspaceTypeInfo() == typeid(WorkspaceType));
			newTool->Initialize(static_cast<WorkspaceType*>(this));
			m_Tools.push_back(static_cast<ITool*>(newTool));
		}

		virtual void Render() override
		{
			ImGuiStyle& style = ImGui::GetStyle();
			ImGuiStyle oldStyle = style;

			ImGuiWindowClass workspaceWindowClass{};
			workspaceWindowClass.ClassId = ImHashStr("WorkspaceStackClass");
			workspaceWindowClass.DockingAllowUnclassed = false;

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

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
			ImGui::DockSpace(ImGui::GetID(windowName.c_str()), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None, &m_ToolWindowClass);
			ImGui::PopStyleVar(1);

			for (ITool* tool : m_Tools)
			{
				tool->BeginRender();
				tool->Render();
				tool->EndRender();
			}

			if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent)) m_ShowActionSearchTool = !m_ShowActionSearchTool;
			if (m_ShowActionSearchTool) m_ActionSearchTool.Render();

			ImGui::End();

			style = oldStyle;
		}
		
		virtual const std::vector<Action*>& GetActions() const override
		{
			return actions;
		};

		virtual const std::vector<ITool*>& GetTools() const override
		{
			return m_Tools;
		}

	protected:
		const std::string m_Name;
		const std::string m_DisplayName;
		ImGuiWindowClass m_ToolWindowClass;

		ActionSearchTool m_ActionSearchTool;
		bool m_ShowActionSearchTool;

		std::vector<Action*> actions;

		std::vector<ITool*> m_Tools;
	};

}