#pragma once

#include <string>
#include "Events/Event.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
		unsigned int Width;
		unsigned int Height;
		std::function<void(Events::Event&)> EventCallback;
	};

	class Window
	{
	public:
		Window(const WindowConfig& config);
		~Window();

		void OnUpdate();
		void SetEventCallbackFunction(const std::function<void(Events::Event&)>& callback);

		void SetVSync(bool active);
		bool IsVSyncActive() const;

		inline unsigned int GetWidth() { return m_config.Width; }
		inline unsigned int GetHeight() { return m_config.Height; }
		inline bool IsWindowActive() { return !glfwWindowShouldClose(m_window); }

		static Window* Create(const WindowConfig& config = WindowConfig());

	private:
		void SetGLFWCallbacks();

	private:
		WindowConfig m_config;
		WindowData m_windowData;
		GLFWwindow* m_window;
		bool m_bVsync;
	};
}