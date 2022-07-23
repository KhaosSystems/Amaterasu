#include "pch.h"

#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

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
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		if (!glfwInit())
			return;

		glfwSetErrorCallback(GLFWErrorCallback);

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

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
			glfwPollEvents();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGuiRender();
			ImGui::EndFrame();
			ImGui::Render();

			int display_w, display_h;
			glfwGetFramebufferSize(m_Window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			glfwSwapBuffers(m_Window);
		}

		glfwTerminate();
	}

	void Application::ImGuiRender()
	{
		ImGuiIO& io = ImGui::GetIO();

		{
			int windowX, windowY = 0;
			glfwGetWindowPos(GetWindow(), &windowX, &windowY);
			ImVec2 windowPos = ImVec2(windowX, windowY);
			ImVec2 p1 = ImVec2(300.0f, 0.0f) + windowPos;
			ImVec2 p2 = ImVec2(600.0f, 60.0f) + windowPos;
			ImVec2 point = io.MousePos;

			if (ImGui::IsMouseClicked(0))
			{
				if (point.x >= p1.x && point.x <= p2.x && point.y >= p1.y && point.y <= p2.y)
				{
					windowMoveOffset = windowPos - point;
					isDraggingWindow = true;
				}
			}

			if (ImGui::IsMouseReleased(0))
			{
				isDraggingWindow = false;
			}

			if (isDraggingWindow)
			{
				glfwSetWindowPos(GetWindow(), point.x + windowMoveOffset.x, point.y + windowMoveOffset.y);
			}
		}

		m_WorkspaceStack.Render();
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

		// Load default font
		// TODO: Don't use absolute paths...
		io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Dev/KhaosSystems/Raijin/Vendor/Amaterasu/Assets/Consolas/CONSOLA.TTF", 14.0f);

		// Load icon fonts
		// https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-can-i-easily-use-icons-in-my-application
		// https://github.com/ocornut/imgui/blob/master/docs/FONTS.md#using-custom-glyph-ranges
		// TODO: Don't use absolute paths...
		ImFontConfig fontConfig{};
		fontConfig.MergeMode = true;
		static ImWchar brandRanges[] = { 0xF3E2, 0xF3E2, 0 };
		static ImWchar solidRanges[] = { 0xF120, 0xF120, 0 };
		io.Fonts->AddFontFromFileTTF("C:/Dev/KhaosSystems/Raijin/Vendor/Amaterasu/Assets/FontAwesome/fa-brands-400.ttf", 14.0f, &fontConfig, brandRanges);
		io.Fonts->AddFontFromFileTTF("C:/Dev/KhaosSystems/Raijin/Vendor/Amaterasu/Assets/FontAwesome/fa-solid-900.ttf", 14.0f, &fontConfig, solidRanges);

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
		style.FrameRounding = 4;
		style.GrabRounding = 4;
		// Alignment
        style.WindowMenuButtonPosition = ImGuiDir_None;
		// Colors
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImColor(21, 21, 21, 255);
		colors[ImGuiCol_Button] = ImColor(30, 30, 30, 255);
		colors[ImGuiCol_FrameBg] = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_FrameBgHovered] = ImColor(65, 65, 65, 255);
		colors[ImGuiCol_FrameBgActive] = ImColor(45, 45, 45, 255);
		colors[ImGuiCol_TitleBg] = ImColor(0, 0, 0, 0);
		colors[ImGuiCol_TitleBgActive] = ImColor(0, 0, 0, 0);
		colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 0);
		colors[ImGuiCol_ButtonHovered] = ImColor(40, 40, 40, 255);
		colors[ImGuiCol_ButtonActive] = ImColor(60, 60, 60, 255);
		colors[ImGuiCol_Header] = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_HeaderHovered] = ImColor(45, 45, 45, 255);
		colors[ImGuiCol_HeaderActive] = ImColor(65, 65, 65, 255);
		colors[ImGuiCol_Separator] = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_SeparatorHovered] = ImColor(45, 45, 45, 255);
		colors[ImGuiCol_SeparatorActive] = ImColor(55, 55, 55, 255);
		colors[ImGuiCol_Tab] = ImColor(0, 0, 0, 0);
		colors[ImGuiCol_TabHovered] = ImColor(36, 36, 36, 255);
		colors[ImGuiCol_TabActive] = ImColor(36, 36, 36, 255);
		colors[ImGuiCol_TabUnfocused] = ImColor(0, 0, 0, 0);
		colors[ImGuiCol_TabUnfocusedActive] = ImColor(36, 36, 36, 255);
		colors[ImGuiCol_CheckMark] = ImColor(160, 160, 160, 255);
	}
}
