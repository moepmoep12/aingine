#pragma once
#include "game.h"
#include <iostream>
#include "UI/ImGuiLayer.h"

AIngine::Application* AIngine::CreateApplication() {
	return new Game();
}

Game::Game()
{
	PushOverlay(new AIngine::UI::ImGuiLayer());
}

Game::~Game()
{
	std::cout << "destructor game \n";
	int x;
	std::cin >> x;
}
