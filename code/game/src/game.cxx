#pragma once
#include "game.h"
#include "Layer.h"
#include "Core.h"
#include "Rendering/texture.h"
#include <random>

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

	//texture = new Texture2D();
	//texture->Generate(bitmap->GetBitmap());

	//m_sceneGraph->AddShape(texture, nullptr);
	{
		//using namespace AIngine::Rendering;
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


		/*ShapeNode* sprite = m_sceneGraph->AddShape(texture);
		sprite->SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
		sprite->SetName(std::string("GreenFace"));

		GroupNode* groupTwo = m_sceneGraph->AddGroup(nullptr);
		groupTwo->SetName(std::string("Parent"));

		ShapeNode* secondSprite = m_sceneGraph->AddShape(texture,groupTwo);
		secondSprite->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
		secondSprite->SetPosition(glm::vec2(500, 300));
		secondSprite->SetName("RedFace");*/

		//AIngine::GameObject * firstObject = m_sceneGraph->SpawnObject(std::string("First Object"));
		//AIngine::GameObject* secondObject = m_sceneGraph->SpawnObject(std::string("SecondObejct"), firstObject);
		//AIngine::Rendering::Texture2D* texture = secondObject->AddComponent<AIngine::Rendering::Texture2D>();
		//texture->Generate(bitmap->GetBitmap());
	}

}

void Game::OnAppShutDown()
{
	DEBUG_INFO("OnAppShutdown");
	//delete texture;
}

void Game::OnAppUpdate()
{
	// spawning sprites with random shapes
	if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::SPACE)) {
		Application& app = AIngine::Application::Get();
		int width = app.GetWindow().GetWidth();
		int height = app.GetWindow().GetHeight();
		static float maxSizeX = 2.0;
		static float maxSizeY = 2.0;
		static float minSizeX = 0.1;
		static float minSizeY = 0.1;
		static float minRot = -M_PI;
		static float maxRot = M_PI;
		static std::string path("assets/game/textures/awesomeface.png");


		AIngine::GameObject* obj = m_sceneGraph->SpawnObject();
		float posX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / width));
		float posY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / height));
		obj->SetLocalPosition(glm::vec2(posX, posY));

		float sizeX = minSizeX + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxSizeX));
		float sizeY = minSizeY + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxSizeY));
		obj->SetLocalScale(glm::vec2(sizeX, sizeY));

		float rot = minRot + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxRot));
		obj->SetRotation(rot);

		AIngine::Rendering::Texture2D* texture = obj->AddComponent<AIngine::Rendering::Texture2D>();
		AIngine::Assets::BitmapAsset* bitmap = app.GetAssetRegistry().Load<AIngine::Assets::BitmapAsset>(path);
		texture->Generate(bitmap->GetBitmap());

		float red = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float green = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float blue = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		obj->GetComponent<AIngine::Rendering::Texture2D>()->SetColor(glm::vec3(red, green, blue));
	}
}

