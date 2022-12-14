#include "AIngine/Core.h"
#include "AIngine/GameObject.h"
#include "Player.h"
#include "Ball.h"
#include "HumanPlayer.h"
#include "GameManager.h"
#include "RandomAgent.h"
#include "XCSAgent.h"
#include "Experiment.h"
#include "MainMenu.h"
#include <vector>
#include <string>

std::vector<std::string> AIngine::ApplicationComponentNames = {
    "Player",
    "Ball",
    "HumanPlayer",
    "GameManager",
    "RandomAgent",
    "XCSAgent",
    "Experiment",
    "MainMenu",
};

void AIngine::OnAddComponent(AIngine::GameObject* obj, int index) {
	switch (index) {
	case 0:
		obj->AddComponent<Pong::Player>()->ScriptIndex = 0;
		break;

	case 1:
		obj->AddComponent<Pong::Ball>()->ScriptIndex = 1;
		break;

	case 2:
		obj->AddComponent<Pong::HumanPlayer>()->ScriptIndex = 2;
		break;

	case 3:
		obj->AddComponent<Pong::GameManager>()->ScriptIndex = 3;
		break;

	case 4:
		obj->AddComponent<Pong::RandomAgent>()->ScriptIndex = 4;
		break;

	case 5:
		obj->AddComponent<Pong::XCSAgent>()->ScriptIndex = 5;
		break;

	case 6:
		obj->AddComponent<Pong::Experiment>()->ScriptIndex = 6;
		break;

	case 7:
		obj->AddComponent<Pong::MainMenu>()->ScriptIndex = 7;
		break;

	}
}