#pragma once
#include "Launcher.h"
#include "EntryPoint.h"

AIngine::Application* AIngine::CreateApplication() {
	return new Launcher();
}

// deprecated. will delete soon
std::vector<std::string> AIngine::ApplicationComponentNames = {};
void AIngine::OnAddComponent(AIngine::GameObject* obj, int index) {}

Launcher::Launcher()
{
	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primary);
	m_window->SetWindowSize(mode->width, mode->height);
	m_window->SetWindowTitle("Launcher");
}
