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
	Application& app = AIngine::Application::Get();
	std::string path("assets/game/textures/awesomeface.png");
	AIngine::Assets::BitmapAsset* bitmap = app.GetAssetRegistry().Load<AIngine::Assets::BitmapAsset>(path);

	texture = new Texture2D();
	texture->Generate(bitmap->GetBitmap());

	//m_sceneGraph->AddShape(texture, nullptr);
	{
		using namespace AIngine::Rendering;
		//GroupNode* two = m_sceneGraph->AddGroup(nullptr);
		//SceneNode* three = m_sceneGraph->AddShape(texture, two);
		//GroupNode* four = m_sceneGraph->AddGroup(two);
		//GroupNode* five = m_sceneGraph->AddGroup(four);
		//ShapeNode* six = m_sceneGraph->AddShape(nullptr, five);
		//ShapeNode* seven = m_sceneGraph->AddShape(nullptr, five);
		//ShapeNode* eight = m_sceneGraph->AddShape(nullptr, four);

		//m_sceneGraph->RemoveNode(five);
		//m_sceneGraph->RemoveNode(eight);

		//GroupNode* two = m_sceneGraph->AddGroup(nullptr);
		//GroupNode* three = m_sceneGraph->AddGroup(nullptr);
		//ShapeNode* four = m_sceneGraph->AddShape(texture, two);


		ShapeNode* sprite = m_sceneGraph->AddShape(texture);
		sprite->SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
		sprite->SetName(std::string("GreenFace"));

		GroupNode* groupTwo = m_sceneGraph->AddGroup(nullptr);
		groupTwo->SetName(std::string("Parent"));

		ShapeNode* secondSprite = m_sceneGraph->AddShape(texture,groupTwo);
		secondSprite->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
		secondSprite->SetPosition(glm::vec2(500, 300));
		secondSprite->SetName("RedFace");
	}

}

void Game::OnAppShutDown()
{
	DEBUG_INFO("OnAppShutdown");
	delete texture;
}

void Game::OnAppUpdate()
{
}

