#pragma once

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#if defined( _WIN32 )
#include <Windows.h>
#endif // _WIN32

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

#if defined(_WIN32)
        LRESULT CustomWindowProc(HWND Handle, UINT Msg, WPARAM WParam, LPARAM LParam);
#endif

    protected:
        WorkspaceStack m_WorkspaceStack;

    private:
        const std::string m_Name;

        GLFWwindow* m_Window;
    
        ImVec2 windowMoveOffset = ImVec2(0.0f, 0.0f);
        ImVec2 originalWindowSize = ImVec2(0.0f, 0.0f);
        ImVec2 lastClickPosition = ImVec2(0.0f, 0.0f);
        bool isDraggingWindow = false;
        bool isResizingWindow = false;
        
    };
}