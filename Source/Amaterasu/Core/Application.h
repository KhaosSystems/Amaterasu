#pragma once

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

    private:
        const std::string m_Name;

        GLFWwindow* m_Window;
    };
}