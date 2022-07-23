#include "Amaterasu/Core/Application.h"

#include <iostream>

#include <imgui.h>
#include <imgui_internal.h>

#include <concepts>
#include <type_traits>

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <iostream>

#include "NodeGraph/NodeGraphDemoWorkspace.h"

namespace AmaterasuDemo
{
	class AmaterasuDemoApplication : public Amaterasu::Application
	{
	public:
		AmaterasuDemoApplication();

	private:
		NodeGraphDemoWorkspace m_NodeGraphDemoWorkspace;
	};

	AmaterasuDemoApplication::AmaterasuDemoApplication()
		: Amaterasu::Application("Amaterasu Demo"), m_NodeGraphDemoWorkspace()
	{
		m_WorkspaceStack.RegisterWorkspace(&m_NodeGraphDemoWorkspace);
	}
}

int main(int argc, char* argv[])
{
	AmaterasuDemo::AmaterasuDemoApplication application = AmaterasuDemo::AmaterasuDemoApplication();
	application.Run();
}