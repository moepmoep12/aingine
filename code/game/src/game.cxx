#pragma once
#include "game.h"
#include "Layer.h"
#include "Core.h"

AIngine::Application* AIngine::CreateApplication() {
	return new Game();
}


class ExampleLayer :public  AIngine::Layer {


public:
	ExampleLayer() : Layer("ExampleLayer") {

	}

	virtual void OnEvent(AIngine::Events::Event& e) override
	{
		DEBUG_INFO(e.ToString().c_str());
	}

	~ExampleLayer() {
		DEBUG_INFO("Destructor ExampleLayer");
	}

};

Game::Game()
{
	DEBUG_WARN("Creating Game...");
	PushLayer(new ExampleLayer());
}

Game::~Game()
{
	DEBUG_WARN("Destructor Game");
}

