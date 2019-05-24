#pragma once

#include <string>
#include "Events/Event.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace AIngine {

	struct WindowConfig {
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowConfig(const std::string title = "Application",
			unsigned int width = 1920,
			unsigned int height = 1080)
			: Title(title), Width(width), Height(height) {}
	};

	struct WindowData {
		std::string Title;
		int Width;
		int Height;
		int XPos;
		int YPos;
		bool HasFocus = true;
		std::function<void(Events::Event&)> EventCallback;
	};

	class Window
	{
	public:
		Window(const WindowConfig& config);
		~Window();

		void PollInput();
		void OnUpdate();
		void SetEventCallbackFunction(const std::function<void(Events::Event&)>& callback);

		void SetVSync(bool active);
		bool IsVSyncActive() const;

		inline unsigned int GetWidth() const { return m_windowData.Width; }
		inline unsigned int GetHeight() const { return m_windowData.Height; }
		inline unsigned int GetLeftEdge() const { return m_leftEdge; }
		inline unsigned int GetRightEdge() const { return m_rightEdge; }
		inline unsigned int GetTopEdge() const { return m_topEdge; }
		inline unsigned int GetBottomEdge() const { return m_bottomEdge; }
		inline unsigned int GetX() const { return m_windowData.XPos;; }
		inline unsigned int GetY() const { return m_windowData.YPos; }
		inline bool IsWindowActive() const { return !glfwWindowShouldClose(m_window); }
		virtual void* GetNativeWindow() const { return m_window; }
		glm::vec2 GetMonitorResolution() const;

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