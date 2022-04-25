#pragma once

#include <string>

#include <GLFW/glfw3.h>

namespace Amaterasu
{
    class Application
    {
    public:
        Application(const std::string& name);
        virtual ~Application();
 
    private:
        const std::string m_Name;

        GLFWwindow* m_Window;
    };
}