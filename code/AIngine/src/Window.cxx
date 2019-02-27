#include "Window.h"
#include "log.h"
#include "Events/ApplicationEvents.h"
#include "Events/InputEvents.h"


AIngine::Window::Window(const WindowConfig & config)
{
	m_config.Title = config.Title;
	m_config.Height = config.Height;
	m_config.Width = config.Width;

	ASSERT(glfwInit(), "Could not init glfw");

	// Request an OpenGL Context 4.1 Core Profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Windowed Fullscreen
	glfwWindowHint(GLFW_MAXIMIZED, 1);

	m_window = glfwCreateWindow((int)m_config.Width, (int)m_config.Height, m_config.Title.c_str(), nullptr, nullptr);

	ASSERT(m_window, "Could not create window");

	glfwMakeContextCurrent(m_window);
	glfwSetWindowUserPointer(m_window, &m_windowData);
	SetVSync(true);

	// Load GLAD
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	ASSERT(gladLoadGL(), "Could not load GLAD");

	SetGLFWCallbacks();
}


AIngine::Window::~Window()
{
	CORE_INFO("Destroying the window");
	glfwDestroyWindow(m_window);
}

void AIngine::Window::OnUpdate()
{
	//glClearColor(0.5, 0.1, 1, 1);
	//glClear(GL_COLOR_BUFFER_BIT);
	glfwPollEvents();
	glfwSwapBuffers(m_window);
}

void AIngine::Window::SetEventCallbackFunction(const std::function<void(Events::Event&)>& callback)
{
	m_windowData.EventCallback = callback;
}

void AIngine::Window::SetVSync(bool active)
{
	if (active)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

	m_bVsync = active;
}

bool AIngine::Window::IsVSyncActive() const
{
	return m_bVsync;
}

AIngine::Window* AIngine::Window::Create(const WindowConfig & config)
{
	return new Window(config);
}

void AIngine::Window::SetGLFWCallbacks()
{
	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		data.Width = width;
		data.Height = height;

		AIngine::Events::WindowResizeEvent event(width, height);
		data.EventCallback(event);
	});

	glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		AIngine::Events::WindowCloseEvent event;
		data.EventCallback(event);
	});

	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		switch (action)
		{
		case GLFW_PRESS:
		{
			AIngine::Events::KeyPressedEvent event(key, 0);
			data.EventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			AIngine::Events::KeyReleasedEvent event(key);
			data.EventCallback(event);
			break;
		}
		case GLFW_REPEAT:
		{
			AIngine::Events::KeyPressedEvent event(key, 1);
			data.EventCallback(event);
			break;
		}
		}
	});

	glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		AIngine::Events::KeyTypedEvent event(keycode);
		data.EventCallback(event);
	});

	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		switch (action)
		{
		case GLFW_PRESS:
		{
			AIngine::Events::MouseButtonPressedEvent event(button);
			data.EventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			AIngine::Events::MouseButtonReleasedEvent event(button);
			data.EventCallback(event);
			break;
		}
		}
	});

	glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		AIngine::Events::MouseScrolledEvent event((float)xOffset, (float)yOffset);
		data.EventCallback(event);
	});

	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		AIngine::Events::MouseMovedEvent event((float)xPos, (float)yPos);
		data.EventCallback(event);
	});
}
