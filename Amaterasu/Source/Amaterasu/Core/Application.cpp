#include "pch.h"

#include "Application.h"

#include <GLFW/glfw3.h>
#include <iostream>

namespace Amaterasu
{

	void GLFWErrorHandler(int error_code, const char* description)
	{
		std::cout << description << std::endl;
	}


	Application::Application(const std::string& name)
		: m_Window(nullptr)
	{
		if (!glfwInit())
			return;

		glfwSetErrorCallback(GLFWErrorHandler);

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_Window = glfwCreateWindow(1280, 720, m_Name.c_str(), nullptr, nullptr);
		if (!m_Window)
		{
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(m_Window);

		while (!glfwWindowShouldClose(m_Window))
		{
			glClear(GL_COLOR_BUFFER_BIT);

			glfwSwapBuffers(m_Window);

			glfwPollEvents();
		}

		glfwTerminate();
	}

	Application::~Application()
	{
	}
}
