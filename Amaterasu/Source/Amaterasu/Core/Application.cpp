#include "pch.h"

#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>

namespace Amaterasu
{

	void GLFWErrorCallback(int error_code, const char* description)
	{
		std::cout << description << std::endl;
	}


	Application::Application(const std::string& name)
		: m_Window(nullptr)
	{
		if (!glfwInit())
			return;

		glfwSetErrorCallback(GLFWErrorCallback);

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

		InitializeImGui();

		// Initialize glad
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		while (!glfwWindowShouldClose(m_Window))
		{
			glfwSwapBuffers(m_Window);
			glfwPollEvents();

			glClear(GL_COLOR_BUFFER_BIT);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}
		}

		glfwTerminate();
	}

	Application::~Application()
	{
	}

	void Application::InitializeImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// Initialize ImGui platform backends
		ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
		ImGui_ImplOpenGL3_Init("#version 130");

		// Load fonts
		// TODO: Don't use absolute paths...
		//io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Dev/Kronos/KronosEditor/Assets/Fonts/Consolas/CONSOLA.TTF", 14.0f);

		// Configure style variables
		ImGuiStyle& style = ImGui::GetStyle();
		// Main
		style.IndentSpacing = 16;
		style.ScrollbarSize = 12;
		// Borders
		style.WindowBorderSize = 1;
		style.PopupBorderSize = 0;
		style.FrameBorderSize = 0;
		style.TabBorderSize = 0;
		style.ChildBorderSize = 0;
		// Rounding
		style.FrameRounding = 2;
		style.GrabRounding = 2;
		// Alignment
		style.WindowMenuButtonPosition = ImGuiDir_Right;
		// Colors
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImColor(30, 30, 30, 255);
		colors[ImGuiCol_Button] = ImColor(30, 30, 30, 255);
		colors[ImGuiCol_FrameBg] = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_FrameBgHovered] = ImColor(65, 65, 65, 255);
		colors[ImGuiCol_FrameBgActive] = ImColor(45, 45, 45, 255);
		colors[ImGuiCol_TitleBg] = ImColor(0, 0, 0, 255);
		colors[ImGuiCol_TitleBgActive] = ImColor(0, 0, 0, 255);
		colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 255);
		colors[ImGuiCol_ButtonHovered] = ImColor(40, 40, 40, 255);
		colors[ImGuiCol_ButtonActive] = ImColor(60, 60, 60, 255);
		colors[ImGuiCol_Header] = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_HeaderHovered] = ImColor(45, 45, 45, 255);
		colors[ImGuiCol_HeaderActive] = ImColor(65, 65, 65, 255);
		colors[ImGuiCol_Separator] = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_SeparatorHovered] = ImColor(45, 45, 45, 255);
		colors[ImGuiCol_SeparatorActive] = ImColor(55, 55, 55, 255);
		colors[ImGuiCol_Tab] = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_TabHovered] = ImColor(65, 65, 65, 255);
		colors[ImGuiCol_TabActive] = ImColor(45, 45, 45, 255);
		colors[ImGuiCol_TabUnfocused] = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_TabUnfocusedActive] = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_CheckMark] = ImColor(160, 160, 160, 255);
	}
}
