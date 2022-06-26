#pragma once

#include <vector>
#include <string>

namespace Amaterasu
{
	struct Action
	{
		Action(const std::string& name, void(*function)()) : Name(name), Function(function) {}

		std::string Name;
		void(*Function)();
	};

	class ActionSearchTool
	{
	public:
		virtual void Render();
		void AppendAction(Action action);

	private:
		std::vector<Action> m_Actions;
	};
}