#pragma once

#include <vector>
#include <string>

namespace Amaterasu
{
	struct Action
	{
		Action(const std::string& name, void(*function)(void*), void* data) : name(name), function(function), data(data) {}

		void Execute() const
		{
			function(data);
		}

		std::string name;
		void(*function)(void*);
		void* data;
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