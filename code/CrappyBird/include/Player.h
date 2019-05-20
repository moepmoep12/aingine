#pragma once
#include "AIngine/GameObject.h"

namespace CrappyBird {
	class Player : public AIngine::GameObject
	{
	public:
		Player(AIngine::GameObject* parent, const std::string& name);
		Player() = delete;
		virtual ~Player();
	};
}