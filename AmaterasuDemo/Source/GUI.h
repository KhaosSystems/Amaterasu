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
            ImGui::Begin("Workspace");
            ImGui::End();
        } 
    };
}