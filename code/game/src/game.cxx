#pragma once
#include "game.h"

AIngine::Application* AIngine::CreateApplication() {
	return new Game();
}