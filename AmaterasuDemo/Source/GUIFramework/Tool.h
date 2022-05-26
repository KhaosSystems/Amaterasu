#pragma once

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <string>

namespace AmaterasuDemo
{
    class Tool
    {
    public:
        Tool(const std::string& name);

        virtual void Render();

    private:
        const std::string m_Name;
    };
}