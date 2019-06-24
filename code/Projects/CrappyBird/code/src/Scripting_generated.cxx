#include "AIngine/Core.h"
#include "AIngine/GameObject.h"
#include "Player.h"
#include "BackGround.h"
#include "PickUpFactory.h"
#include "Obstacles.h"
#include "PickUp.h"
#include <vector>
#include <string>

std::vector<std::string> AIngine::ApplicationComponentNames = {
    "Player",
    "BackGround",
    "PickUpFactory",
    "Obstacles",
    "PickUp",
};

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