#pragma once

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace AmaterasuDemo
{
    class Workspace
    {
    public:
        void Render()
        {
			ImGuiStyle& style = ImGui::GetStyle();
			ImGuiStyle oldStyle = style;

			style.WindowMenuButtonPosition = ImGuiDir_Left;
			style.FramePadding = ImVec2(8.0f, 8.0f);
			style.WindowPadding = ImVec2(0.0f, 0.0f);

			ImGuiWindowClass workspaceClass;
			workspaceClass.ClassId = ImHashStr("Workspace");

			workspaceClass.DockingAllowUnclassed = false;
			ImGui::SetNextWindowClass(&workspaceClass);
			ImGui::Begin("SceneEditorWorkspaceWindow", (bool*)0, ImGuiDockNodeFlags_None);

			ImDrawList* drawList = ImGui::GetWindowDrawList();
			ImVec2 p1 = ImGui::GetCursorScreenPos();
			ImVec2 p2 = p1 + ImVec2(ImGui::GetContentRegionAvail().x + (style.WindowPadding.x * 2), 3.0f);
			drawList->AddRectFilled(p1, p2, IM_COL32(36, 36, 36, 255));
			ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.0f, 3.0f));

			ImGuiWindowClass sceneToolClass;
			sceneToolClass.ClassId = ImHashStr("SceneToolClass");
			sceneToolClass.DockingAllowUnclassed = false;

			style.WindowMenuButtonPosition = ImGuiDir_None;
			style.FramePadding = ImVec2(4.0f, 4.0f);
			ImGui::DockSpace(ImGui::GetID("SceneEditorWorkspaceWindow"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None, &sceneToolClass);

			// Scene Editor Viewport
			{
				style.WindowMenuButtonPosition = ImGuiDir_None;
				style.FramePadding = ImVec2(4.0f, 4.0f);
				ImGui::SetNextWindowClass(&sceneToolClass);
				ImGui::Begin("SceneEditorViewport");
				ImGui::End();
			}

			// Scene Editor Hierarchy
			{
				style.WindowMenuButtonPosition = ImGuiDir_None;
				style.FramePadding = ImVec2(4.0f, 4.0f);
				ImGui::SetNextWindowClass(&sceneToolClass);
				ImGui::Begin("SceneEditorHierarchy");
				ImGui::End();
			}

			ImGui::End();

			style = oldStyle;
        } 
    };
}