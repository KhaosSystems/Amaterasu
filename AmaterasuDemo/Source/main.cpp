#include "Amaterasu/Core/Application.h"

#include <iostream>
#include <imgui.h>

namespace AmaterasuDemo
{
    class AmaterasuDemoApplication : public Amaterasu::Application
    {
    public:
        AmaterasuDemoApplication();

        void ImGuiRender() override;
    };

    AmaterasuDemoApplication::AmaterasuDemoApplication()
        : Amaterasu::Application("Amaterasu Demo")
    {
        std::cout << "called2" << std::endl;
    }

    void AmaterasuDemoApplication::ImGuiRender()
    {
        Application::ImGuiRender();

        std::cout << "called" << std::endl;
        ImGui::ShowDemoWindow();
    }
}

int main(int argc, char* argv[])
{
    AmaterasuDemo::AmaterasuDemoApplication application = AmaterasuDemo::AmaterasuDemoApplication();
    application.Run();
}