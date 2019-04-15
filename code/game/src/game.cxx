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
		//DEBUG_INFO(e.ToString().c_str());
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

void Game::OnAppStartUp()
{
	DEBUG_INFO("OnAppStartUp");

	AIngine::Application& app = AIngine::Application::Get();

	std::string vertexPath("assets/Intellgine/shader/unlit/vertex.glsl");
	std::string fragPath("assets/Intellgine/shader/unlit/fragment.glsl");
	std::string path;
	path.append(vertexPath);
	path.append(";");
	path.append(fragPath);

	AIngine::Assets::ShaderAsset* shader = app.GetAssetRegistry().Load<AIngine::Assets::ShaderAsset>(path);
	int uid = shader->uid;

	DEBUG_INFO("Loaded ShaderProgram with {0} " , shader->GetShader().GetID());

	AIngine::Assets::ShaderAsset* sameShader = app.GetAssetRegistry().Load<AIngine::Assets::ShaderAsset>(path);

	DEBUG_INFO("Loaded the same ShaderProgram with ID {0} ", sameShader->GetShader().GetID());

}

void Game::OnAppShutDown()
{
	DEBUG_INFO("OnAppShutdown");
}

