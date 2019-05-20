#include "Player.h"
#include "Rendering/texture.h"
#include <string>
#include "AIngine/Physics.h"
#include "Application.h"

namespace CrappyBird {

	CrappyBird::Player::Player(AIngine::GameObject * parent, const std::string & name)
		: AIngine::GameObject(parent, name)
	{

	}

	CrappyBird::Player::~Player()
	{
	}
}