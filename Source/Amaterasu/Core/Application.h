#pragma once

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Amaterasu/GUIFramework/WorkspaceStack.h"

namespace Amaterasu
{
    class Application
    {
    public:
        Application(const std::string& name);
        virtual ~Application();
 
        void Run();

        virtual void ImGuiRender();

        GLFWwindow* GetWindow() const { return m_Window; }

    private:
        void InitializeImGui();

    protected:
        WorkspaceStack m_WorkspaceStack;

    private:
        const std::string m_Name;

        GLFWwindow* m_Window;
    
        ImVec2 windowMoveOffset, originalWindowSize, lastClickPosition = ImVec2(0.0f, 0.0f);
        bool isDraggingWindow, isResizingWindow = false;
        
    };
}