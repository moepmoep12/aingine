#pragma once

#include <string>
#include "Events/Event.h"
#include "Events/ApplicationEvents.h"
#include "Events/InputEvents.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace AIngine {

	struct WindowConfig {
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		bool startFullScreen;

		WindowConfig(const std::string title = "Application",
			unsigned int width = 1920,
			unsigned int height = 1080)
			: Title(title), Width(width), Height(height), startFullScreen(true) {}
	};

	struct WindowData {
		std::string Title;
		int Width;
		int Height;
		int XPos;
		int YPos;
		bool HasFocus = true;
		Events::WindowCloseEvent OnWindowClosedEvent;
		Events::WindowFocusEvent OnWindowFocusEvent;
		Events::WindowFocusLostEvent OnWindowFocusLostEvent;
		Events::WindowMovedEvent OnWindowMovedEvent;
		Events::WindowResizeEvent OnWindowResizeEvent;
		Events::KeyPressedEvent OnKeyPressedEvent;
		Events::KeyReleasedEvent OnKeyReleasedEvent;
		Events::KeyTypedEvent OnKeyTypedEvent;
		Events::MouseButtonPressedEvent OnMouseButtonPressedEvent;
		Events::MouseButtonReleasedEvent OnMouseButtonReleasedEvent;
		Events::MouseScrolledEvent OnMouseScrolledEvent;
		Events::MouseMovedEvent OnMouseMovedEvent;

	};

	class Window final
	{
	public:

		enum class MouseCursor {
			Normal = 0,
			Arrow,
			Text,
			Crosshair,
			Hand,
			ResizeHorizontal,
			ResizeVertical
		};

		Window(const WindowConfig& config);
		~Window();

		void PollInput();
		void OnUpdate();

		void SetVSync(bool active);
		bool IsVSyncActive() const;

		inline unsigned int GetWidth() const { return m_windowData.Width; }
		inline unsigned int GetHeight() const { return m_windowData.Height; }
		inline unsigned int GetLeftEdge() const { return m_leftEdge; }
		inline unsigned int GetRightEdge() const { return m_rightEdge; }
		inline unsigned int GetTopEdge() const { return m_topEdge; }
		inline unsigned int GetBottomEdge() const { return m_bottomEdge; }
		inline  int GetX() const { return m_windowData.XPos; }
		inline  int GetY() const { return m_windowData.YPos; }
		inline bool IsWindowActive() const { return !glfwWindowShouldClose(m_window); }
		bool IsFullScreen() const;
		virtual void* GetNativeWindow() const { return m_window; }
		glm::vec2 GetMonitorResolution() const;
		float GetAspectRatio() const;
		HWND GetWin32Window() const;
		void SetIcon(int width, int height, unsigned char* data);
		void SetWindowTitle(const char* title);
		void AppendWindowTitle(const char* append);
		void SetWindowSize(int w, int h);
		void SetWindowSizeLimits(int wMin, int wMax, int hMin, int hMax);
		void SetWindowResize(bool enabled);
		void SetWindowPosition(const glm::vec2& pos);
		void SetMouseVisible(bool visible) const;
		void SetMouseCursor(MouseCursor mode) const;
		void SetFullScreen(bool fullscreen);
		void Close();
		inline WindowData& GetWindowData() { return m_windowData; }

		static Window* Create(const WindowConfig& config = WindowConfig());

	private:
		void SetGLFWCallbacks();

	private:
		WindowConfig m_config;
		WindowData m_windowData;
		GLFWwindow* m_window;
		bool m_bVsync;
		int m_leftEdge, m_rightEdge, m_topEdge, m_bottomEdge;
	};
}