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

bool AIngine::Window::IsFullScreen() const
{
	return glfwGetWindowMonitor(m_window) != nullptr;
}

glm::vec2 AIngine::Window::GetMonitorResolution() const
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return glm::vec2(mode->width, mode->height);
}

float AIngine::Window::GetAspectRatio() const
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return (float)mode->width / (float)mode->height;
}

HWND AIngine::Window::GetWin32Window() const
{
	return glfwGetWin32Window(m_window);
}

void AIngine::Window::SetIcon(int width, int height, unsigned char * data)
{
	GLFWimage img[1];
	img[0].height = height;
	img[0].width = width;
	img[0].pixels = data;
	glfwSetWindowIcon(m_window, 1, img);
}



void AIngine::Window::SetWindowTitle(const char * title)
{
	glfwSetWindowTitle(m_window, title);
	m_windowData.Title = title;
}

void AIngine::Window::AppendWindowTitle(const char * append)
{
	std::string Title(m_windowData.Title);
	Title.append(append);
	SetWindowTitle(Title.c_str());
}

void AIngine::Window::SetWindowSize(int w, int h)
{
	glfwSetWindowSize(m_window, w, h);
	glfwGetWindowSize(m_window, &m_windowData.Width, &m_windowData.Height);
}

void AIngine::Window::SetWindowSizeLimits(int wMin, int wMax, int hMin, int hMax)
{
	glfwSetWindowSizeLimits(m_window, wMin, wMax, hMin, hMax);
}

void AIngine::Window::SetWindowResize(bool enabled)
{
	glfwWindowHint(GLFW_RESIZABLE, enabled);
}

void AIngine::Window::SetWindowPosition(const glm::vec2 & pos)
{
	glfwSetWindowPos(m_window, pos.x, pos.y);
	glfwGetWindowSize(m_window, &m_windowData.Width, &m_windowData.Height);
	glfwGetWindowFrameSize(m_window, &m_leftEdge, &m_topEdge, &m_rightEdge, &m_bottomEdge);
	glfwGetWindowPos(m_window, &m_windowData.XPos, &m_windowData.YPos);
	m_windowData.OnWindowMovedEvent(pos.x, pos.y);
}

void AIngine::Window::SetMouseVisible(bool visible) const
{
	if (!visible)
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	else
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void AIngine::Window::SetMouseCursor(MouseCursor mode) const
{
	int value = 0;
	switch (mode) {
	case MouseCursor::Arrow:
		value = GLFW_ARROW_CURSOR;
		break;
	case MouseCursor::Crosshair:
		value = GLFW_CROSSHAIR_CURSOR;
		break;
	case MouseCursor::Hand:
		value = GLFW_HAND_CURSOR;
		break;
	case MouseCursor::Normal:
		value = GLFW_CURSOR_NORMAL;
		break;
	case MouseCursor::ResizeHorizontal:
		value = GLFW_HRESIZE_CURSOR;
		break;
	case MouseCursor::ResizeVertical:
		value = GLFW_VRESIZE_CURSOR;
		break;
	case MouseCursor::Text:
		value = GLFW_IBEAM_CURSOR;
		break;
	default:
		value = GLFW_CURSOR_NORMAL;
	}

	glfwSetInputMode(m_window, GLFW_CURSOR, value);
}

void AIngine::Window::SetFullScreen(bool fullscreen)
{
	if (IsFullScreen() == fullscreen)
		return;

	if (fullscreen)
	{
		// get reolution of monitor
		const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		// swithc to full screen
		glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, 0);
		if (IsVSyncActive())
			SetVSync(true);
	}
	else
	{
		// restore last window size and position
		glfwSetWindowMonitor(m_window, nullptr, m_windowData.XPos, m_windowData.YPos, m_windowData.Width, m_windowData.Height, 0);
	}

	glfwMakeContextCurrent(m_window);
	glfwGetWindowSize(m_window, &m_windowData.Width, &m_windowData.Height);
	glfwGetWindowFrameSize(m_window, &m_leftEdge, &m_topEdge, &m_rightEdge, &m_bottomEdge);
	glfwGetWindowPos(m_window, &m_windowData.XPos, &m_windowData.YPos);
}

void AIngine::Window::Close()
{
	glfwSetWindowShouldClose(m_window, true);
	m_windowData.OnWindowClosedEvent();
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
			data.OnKeyPressedEvent((AIngine::KeyCode)key);
			break;
		}
		case GLFW_RELEASE:
		{
			//AIngine::Events::KeyReleasedEvent event(key);
			//data.EventCallback(event);
			data.OnKeyReleasedEvent((AIngine::KeyCode)key);
			break;
		}
		case GLFW_REPEAT:
		{
			//AIngine::Events::KeyPressedEvent event(key, 1);
			//data.EventCallback(event);
			data.OnKeyPressedEvent((AIngine::KeyCode)key);
			break;
		}
		}
	});

	glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		//AIngine::Events::KeyTypedEvent event(keycode);
		//data.EventCallback(event);
		data.OnKeyTypedEvent((AIngine::KeyCode)keycode);
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
			data.OnMouseButtonPressedEvent(static_cast<MouseButton>(button));
			break;
		}
		case GLFW_RELEASE:
		{
			//AIngine::Events::MouseButtonReleasedEvent event(button);
			//data.EventCallback(event);
			data.OnMouseButtonReleasedEvent(static_cast<MouseButton>(button));
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
	int EventHandler<void, unsigned int, unsigned int>::counter = 0;
	int EventHandler<void, float, float>::counter = 0;
	int EventHandler<void, int>::counter = 0;
	int EventHandler<void, AIngine::KeyCode>::counter = 0;
	int EventHandler<void, AIngine::MouseButton>::counter = 0;
}