#include "CrappyBird.h"
#include "Player.h"
#include "BackGround.h"
#include "EntryPoint.h"
#include "PickUp.h"


AIngine::Application* AIngine::CreateApplication() {
	return new CrappyBird::CrappyBird();
}

std::vector<std::string> AIngine::ApplicationComponentNames = { "Player", "BackGround", "PickUpFactory" };

void AIngine::OnAddComponent(AIngine::GameObject* obj, int index) {
	switch (index) {
	case 0:
		obj->AddComponent<CrappyBird::Player>()->ScriptIndex = 0;
		break;

	case 1:
		obj->AddComponent<CrappyBird::BackGround>()->ScriptIndex = 1;
		break;

	case 2:
		obj->AddComponent<CrappyBird::PickUpFactory>()->ScriptIndex = 2;
	}
}

namespace CrappyBird {

	float CrappyBird::s_GameSpeed = 2.5f;

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