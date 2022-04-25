#include "Amaterasu/Core/Application.h"

namespace AmaterasuDemo
{
    class Application : public Amaterasu::Application
    {
    public:
        Application();
    };

    Application::Application()
        : Amaterasu::Application("Amaterasu Demo")
    {
    }
}

int main(int argc, char* argv[])
{
    AmaterasuDemo::Application application = AmaterasuDemo::Application();
}