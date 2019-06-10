#include "Window.h"
#include "AIngine/Macros.h"
#include "Events/ApplicationEvents.h"
#include "Events/InputEvents.h"
#include "AIngine/KeyCodes.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


AIngine::Window::Window(const WindowConfig & config)
{
	CORE_INFO("Creating Window...");

	m_config.Title = config.Title;
	m_config.Height = config.Height;
	m_config.Width = config.Width;
	m_windowData.Height = config.Height;
	m_windowData.Width = config.Width;
	m_windowData.Title = config.Title;


	int success = -1;
	success = glfwInit();

	ASSERT(success, "Could not init glfw");

	// Request an OpenGL Context 4.1 Core Profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Windowed Fullscreen
	glfwWindowHint(GLFW_MAXIMIZED, 1);
	glfwWindowHint(GLFW_SAMPLES, 4);

	m_window = glfwCreateWindow((int)m_config.Width, (int)m_config.Height, m_config.Title.c_str(), nullptr, nullptr);

	ASSERT(m_window, "Could not create window");

	glfwMakeContextCurrent(m_window);
	glfwSetWindowUserPointer(m_window, &m_windowData);
	SetVSync(true);

	// Load GLAD
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	ASSERT(gladLoadGL(), "Could not load GLAD");

	SetGLFWCallbacks();


	// OpenGL configuration
	glViewport(0, 0, config.Width, config.Height);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);

	CORE_INFO("Window created");

	glfwGetWindowSize(m_window, &m_windowData.Width, &m_windowData.Height);
	glfwGetWindowFrameSize(m_window, &m_leftEdge, &m_topEdge, &m_rightEdge, &m_bottomEdge);
	glfwGetWindowPos(m_window, &m_windowData.XPos, &m_windowData.YPos);
}


AIngine::Window::~Window()
{
	CORE_INFO("Destructor Window");
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void AIngine::Window::PollInput()
{
	glfwPollEvents();

}

void AIngine::Window::OnUpdate()
{
	//glClearColor(0.5, 0.1, 1, 1);
	//glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(m_window);
	glClearColor(0.5, 0.5, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
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

glm::vec2 AIngine::Window::GetMonitorResolution() const
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return glm::vec2(mode->width, mode->height);
}

HWND AIngine::Window::GetWin32Window() const
{
	return glfwGetWin32Window(m_window);
}

void AIngine::Window::SetWindowTitle(const char * title)
{
	glfwSetWindowTitle(m_window, title);
	m_windowData.Title = title;
}

void AIngine::Window::SetWindowSize(int w, int h)
{
	glfwSetWindowSize(m_window, w, h);
	m_windowData.Width = w;
	m_windowData.Height = h;
}

AIngine::Window* AIngine::Window::Create(const WindowConfig & config)
{
	return new Window(config);
}

void AIngine::Window::SetGLFWCallbacks()
{
	// WindowSize 
	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		data.Width = width;
		data.Height = height;

		data.OnWindowResizeEvent(width, height);
	});

	// WindowClose
	glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		data.OnWindowClosedEvent();
	});

	// WindowPosition
	glfwSetWindowPosCallback(m_window, [](GLFWwindow* window, int xpos, int ypos) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		data.XPos = xpos;
		data.YPos = ypos;

		data.OnWindowMovedEvent(xpos, ypos);
	});

	// WindowFocus
	glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		data.HasFocus = focused;

		if (focused) {
			data.OnWindowFocusEvent();
		}
		else {
			data.OnWindowFocusLostEvent();
		}
	});

	// Key
	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		switch (action)
		{
		case GLFW_PRESS:
		{
			//AIngine::Events::KeyPressedEvent event(key, 0);
			//data.EventCallback(event);
			data.OnKeyPressedEvent((AIngine::KeyCodes)key);
			break;
		}
		case GLFW_RELEASE:
		{
			//AIngine::Events::KeyReleasedEvent event(key);
			//data.EventCallback(event);
			data.OnKeyReleasedEvent((AIngine::KeyCodes)key);
			break;
		}
		case GLFW_REPEAT:
		{
			//AIngine::Events::KeyPressedEvent event(key, 1);
			//data.EventCallback(event);
			data.OnKeyPressedEvent((AIngine::KeyCodes)key);
			break;
		}
		}
	});

	glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		//AIngine::Events::KeyTypedEvent event(keycode);
		//data.EventCallback(event);
		data.OnKeyTypedEvent((AIngine::KeyCodes)keycode);
	});

	// MouseButton
	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		switch (action)
		{
		case GLFW_PRESS:
		{
			//AIngine::Events::MouseButtonPressedEvent event(button);
			//data.EventCallback(event);
			data.OnMouseButtonPressedEvent(button);
			break;
		}
		case GLFW_RELEASE:
		{
			//AIngine::Events::MouseButtonReleasedEvent event(button);
			//data.EventCallback(event);
			data.OnMouseButtonReleasedEvent(button);
			break;
		}
		}
	});

	// Mouse Scroll
	glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		//AIngine::Events::MouseScrolledEvent event((float)xOffset, (float)yOffset);
		//data.EventCallback(event);
		data.OnMouseScrolledEvent((float)xOffset, (float)yOffset);
	});

	// Mouse Moved
	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		//AIngine::Events::MouseMovedEvent event((float)xPos, (float)yPos);
		//data.EventCallback(event);
		data.OnMouseMovedEvent((float)xPos, (float)yPos);
	});
}

// Initialize static counter
namespace AIngine::Events {
	int EventHandler<void>::counter = 0;
	int EventHandler<void,unsigned int,unsigned int>::counter = 0;
	int EventHandler<void,float,float>::counter = 0;
	int EventHandler<void,int>::counter = 0;
	int EventHandler<void, AIngine::KeyCodes>::counter = 0;
}