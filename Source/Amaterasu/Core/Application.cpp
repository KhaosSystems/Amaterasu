#include "pch.h"

#include "Application.h"

#if defined( _WIN32 )
	#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>

#if defined( _WIN32 )
#include <dwmapi.h>
#endif


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

	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);

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
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		m_Window = glfwCreateWindow(1280, 720, m_Name.c_str(), nullptr, nullptr);
		if (!m_Window)
		{
			glfwTerminate();
			return;
		}

		glfwSetWindowPos(m_Window, 100, 100);
		glfwSetWindowSizeLimits(m_Window, 720, 576, GLFW_DONT_CARE, GLFW_DONT_CARE);
		
		glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);
		
		glfwMakeContextCurrent(m_Window);
		glfwSwapInterval(1); // V-Sync

#if defined(_WIN32)
		
		// https://github.com/Geno-IDE/Geno/blob/master/src/Geno/C%2B%2B/GUI/MainWindow.cpp

		HWND WindowHandle = glfwGetWin32Window(m_Window);

		// Add some necessary window styles manually.
		// First of all, fix the fact that GLFW doesn't set WS_THICKFRAME or WS_MAXIMIZEBOX for non-decorated windows (Required for resizability).
		// Secondly, add WS_CAPTION so that we get a smooth animation when we minimize and maximize the window.
		SetWindowLong(WindowHandle, GWL_STYLE, GetWindowLong(WindowHandle, GWL_STYLE) | WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

		// Fix missing drop shadow
		MARGINS ShadowMargins = { 10, 10, 10, 10 };
		DwmExtendFrameIntoClientArea(WindowHandle, &ShadowMargins);

		// Override window procedure with custom one to allow native window moving behavior without a title bar
		SetWindowLongPtr(WindowHandle, GWLP_USERDATA, (LONG_PTR)this);
		m_DefaultWindowProc = (WNDPROC)SetWindowLongPtr(WindowHandle, GWLP_WNDPROC, (LONG_PTR)CustomWindowProc);

		// Create drop target
		//m_pDropTarget = new Win32DropTarget();
#endif

		// GLFW Init end
		InitializeImGui();

		// Initialize glad
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		while (!glfwWindowShouldClose(m_Window))
		{
			Render();

			glfwPollEvents();
		}

		glfwTerminate();
	}

	static bool b_Rendering = false;
	void Application::Render()
	{
		if (b_Rendering) return;
		b_Rendering = true;

		const ImVec4 Color = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();
		
		ImGuiRender();
		ImGui::EndFrame();

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

		glfwSwapBuffers(m_Window);
	
		b_Rendering = false;
	}

	void Application::ImGuiRender()
	{
		//ImGuiIO& io = ImGui::GetIO();

		// TODO: Better window dragging.

		/*int windowPosX, windowPosY, windowSizeX, windowSizeY = 0;
		glfwGetWindowPos(GetWindow(), &windowPosX, &windowPosY);
		glfwGetWindowSize(GetWindow(), &windowSizeX, &windowSizeY);
		ImVec2 windowPos = ImVec2(windowPosX, windowPosY);
		ImVec2 windowSize = ImVec2(windowSizeX, windowSizeY);
		ImVec2 p1 = ImVec2(160.0f, 0.0f) + windowPos;
		ImVec2 p2 = ImVec2(1170.0f, 30.0f) + windowPos;
		ImVec2 point = io.MousePos;

		//ImGui::GetForegroundDrawList()->AddRectFilled(p1, p2, ImColor(0.0f, 0.0f, 1.0f, 0.5f));


		if (ImGui::IsMouseClicked(0))
		{
			lastClickPosition = point;

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
		}*/

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
		io.FontDefault = io.Fonts->AddFontFromFileTTF("D:/Dev/KhaosSystems/Amaterasu/Assets/Consolas/CONSOLA.TTF", 14.0f);

		// Load icon fonts
		// https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-can-i-easily-use-icons-in-my-application
		// https://github.com/ocornut/imgui/blob/master/docs/FONTS.md#using-custom-glyph-ranges
		// TODO: Don't use absolute paths...
		ImFontConfig fontConfig{};
		fontConfig.MergeMode = true;
		static ImWchar brandRanges[] = { 0xF3E2, 0xF3E2, 0 };
		static ImWchar solidRanges[] = { 0xF120, 0xF120, 0 };
		io.Fonts->AddFontFromFileTTF("D:/Dev/KhaosSystems/Amaterasu/Assets/FontAwesome/fa-brands-400.ttf", 14.0f, &fontConfig, brandRanges);
		io.Fonts->AddFontFromFileTTF("D:/Dev/KhaosSystems/Amaterasu/Assets/FontAwesome/fa-solid-900.ttf", 14.0f, &fontConfig, solidRanges);

		// Configure style variables
		ImGuiStyle& style = ImGui::GetStyle();
		// Main
		style.IndentSpacing = 16;
		style.ScrollbarSize = 12;
		// Borders
		style.WindowBorderSize = 0.5f; // Was 1
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
		colors[ImGuiCol_TitleBg] = ImColor(21, 21, 21, 255);
		colors[ImGuiCol_TitleBgActive] = ImColor(21, 21, 21, 255);
		colors[ImGuiCol_TitleBgCollapsed] = ImColor(21, 21, 21, 255);
		colors[ImGuiCol_ButtonHovered] = ImColor(40, 40, 40, 255);
		colors[ImGuiCol_ButtonActive] = ImColor(60, 60, 60, 255);
		colors[ImGuiCol_Header] = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_HeaderHovered] = ImColor(45, 45, 45, 255);
		colors[ImGuiCol_HeaderActive] = ImColor(65, 65, 65, 255);
		colors[ImGuiCol_Separator] = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_SeparatorHovered] = ImColor(45, 45, 45, 255);
		colors[ImGuiCol_SeparatorActive] = ImColor(55, 55, 55, 255);
		colors[ImGuiCol_Tab] = ImColor(21, 21, 21, 255);
		colors[ImGuiCol_TabHovered] = ImColor(36, 36, 36, 255);
		colors[ImGuiCol_TabActive] = ImColor(36, 36, 36, 255);
		colors[ImGuiCol_TabUnfocused] = ImColor(21, 21, 21, 255);
		colors[ImGuiCol_TabUnfocusedActive] = ImColor(36, 36, 36, 255);
		colors[ImGuiCol_CheckMark] = ImColor(160, 160, 160, 255);
	}

#if defined(_WIN32)
	LRESULT Application::CustomWindowProc(HWND Handle, UINT Msg, WPARAM WParam, LPARAM LParam)
	{
		Application* self = (Application*)GetWindowLongPtr(Handle, GWLP_USERDATA);

		switch (Msg)
		{
		case WM_NCHITTEST:
		{
			POINT MousePos;
			RECT  WindowRect;

			GetCursorPos(&MousePos);
			GetWindowRect(Handle, &WindowRect);

			if (PtInRect(&WindowRect, MousePos))
			{
				const int BorderX = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
				const int BorderY = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);

				if (MousePos.y < (WindowRect.top + BorderY))
				{
					if (MousePos.x < (WindowRect.left + BorderX)) { ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE); return HTTOPLEFT; }
					else if (MousePos.x >= (WindowRect.right - BorderX)) { ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW); return HTTOPRIGHT; }
					else { ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);   return HTTOP; }
				}
				else if (MousePos.y >= (WindowRect.bottom - BorderY))
				{
					if (MousePos.x < (WindowRect.left + BorderX)) { ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW); return HTBOTTOMLEFT; }
					else if (MousePos.x >= (WindowRect.right - BorderX)) { ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE); return HTBOTTOMRIGHT; }
					else { ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);   return HTBOTTOM; }
				}
				else if (MousePos.x < (WindowRect.left + BorderX))
				{
					ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
					return HTLEFT;
				}
				else if (MousePos.x >= (WindowRect.right - BorderX))
				{
					ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
					return HTRIGHT;
				}
				else
				{
					// Drag the menu bar to move the window
					//if (!ImGui::IsAnyItemHovered() && (MousePos.y < (WindowRect.top + self->pTitleBar->Height())))
						//return HTCAPTION;
				}
			}

		} break;

		case WM_NCCALCSIZE:
		{
			// Fix maximized windows for some reason accounting for border size
			if (WParam == TRUE)
			{
				WINDOWPLACEMENT WindowPlacement{ .length = sizeof(WINDOWPLACEMENT) };

				if (GetWindowPlacement(Handle, &WindowPlacement) && WindowPlacement.showCmd == SW_SHOWMAXIMIZED)
				{
					NCCALCSIZE_PARAMS& rParams = *reinterpret_cast<LPNCCALCSIZE_PARAMS>(LParam);
					const int          BorderX = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
					const int          BorderY = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);

					rParams.rgrc[0].left += BorderX;
					rParams.rgrc[0].top += BorderY;
					rParams.rgrc[0].right -= BorderX;
					rParams.rgrc[0].bottom -= BorderY;

					// Use the rectangle specified in rgrc[0] for the new client area
					return WVR_VALIDRECTS;
				}
			}

			// Preserve the old client area and align it with the upper-left corner of the new client area
			return 0;

		} break;

		case WM_ENTERSIZEMOVE:
		{
			SetTimer(Handle, 1, USER_TIMER_MINIMUM, NULL);

		} break;

		case WM_EXITSIZEMOVE:
		{
			KillTimer(Handle, 1);

		} break;

		case WM_TIMER:
		{
			const UINT_PTR TimerID = (UINT_PTR)WParam;

			if (TimerID == 1)
			{
				self->Render();
			}

		} break;

		case WM_SIZE:
		case WM_MOVE:
		{
			self->Render();

		} break;
		}

		return CallWindowProc(self->m_DefaultWindowProc, Handle, Msg, WParam, LParam);
	}
#endif

}
