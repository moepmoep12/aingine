#include "CrappyBird.h"
#include "Player.h"
#include "BackGround.h"
#include "EntryPoint.h"
#include "PickUp.h"
#include "Obstacles.h"


AIngine::Application* AIngine::CreateApplication() {
	return new CrappyBird::CrappyBird();
}

std::vector<std::string> AIngine::ApplicationComponentNames = { "Player", "BackGround", "PickUpFactory", "Obstacles","PickUp" };

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
		break;

	case 3:
		obj->AddComponent<CrappyBird::Obstacles>()->ScriptIndex = 3;
		break;

	case 4:
		obj->AddComponent<CrappyBird::PickUp>()->ScriptIndex = 4;
		break;
	}
}

namespace CrappyBird {

	const float CrappyBird::s_originalGameSpeed = 2.5f;
	float CrappyBird::s_GameSpeed = CrappyBird::s_originalGameSpeed;

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