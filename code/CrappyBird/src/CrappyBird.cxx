#include "CrappyBird.h"
#include "Player.h"
#include <random>
#include <time.h>

AIngine::Application* AIngine::CreateApplication() {
	return new CrappyBird::CrappyBird();
}

std::vector<std::string> AIngine::ApplicationComponentNames = { "Player" };

void AIngine::OnAddComponent(AIngine::GameObject* obj, int index) {
	switch (index) {
	case 0:
		obj->AddComponent<CrappyBird::Player>()->ScriptIndex = 0;
		break;
	}
}

namespace CrappyBird {

	CrappyBird::CrappyBird()
	{
		// configure game world
		m_gravity = (glm::vec2(0.0, 5.0));
		m_bounds = glm::vec4(0.0, 10.0, 0.0, 10.0);
		GLFWmonitor* primary = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primary);
		m_window->SetWindowSize(mode->width, mode->height);
		m_window->SetWindowTitle("CrappyBird");
	}

	CrappyBird::~CrappyBird()
	{
	}

}