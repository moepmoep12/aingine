#pragma once
#include "game.h"
#include "UI/ImGuiLayer.h"

AIngine::Application* AIngine::CreateApplication() {
	return new Game();
}

Game::Game()
{
	DEBUG_WARN("Creating Game...");
	PushOverlay(new AIngine::UI::ImGuiLayer());
}

Game::~Game()
{
	DEBUG_WARN("Destructor Game");
}
