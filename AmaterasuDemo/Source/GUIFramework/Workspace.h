#pragma once

#include "Tool.h"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <string>
#include <deque>

namespace AmaterasuDemo
{
    class Workspace
    {
    public:
        Workspace(const std::string& name);
        
        virtual void Render();

    private:
        const std::string m_Name;
        std::deque<Tool*> m_Tools;
    };
}