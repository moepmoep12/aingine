#pragma once
#include "game.h"
#include "Structures/Layer.h"
//#include "Core.h"
#include "Rendering/texture.h"
#include <glm/glm.hpp>
#include <random>
#include "imgui.h"
#include "Events/InputEvents.h"

AIngine::Application* AIngine::CreateApplication() {
	return new Game();
}

// cam settings
static float translationrate = 5;
static float rotationrate = 0.1;
static float zoomSpeed = 5;


class ExampleLayer :public  AIngine::Layer {


public:
	ExampleLayer() : Layer("ExampleLayer") {

	}

	virtual void OnEvent(AIngine::Events::Event& e) override
	{
		//DEBUG_INFO(e.ToString().c_str());
	}

	virtual void OnImGuiRender() override
	{
	}


	~ExampleLayer() {
		DEBUG_INFO("Destructor ExampleLayer");
	}

};

Game::Game()
{
	DEBUG_WARN("Creating Game...");
	PushLayer(new ExampleLayer());

	m_gravity = (glm::vec2(0.0, 5.0));
	m_bounds = glm::vec4(0.0, 10.0, 10.0, 0.0);

	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primary);
	m_windowConfig.Height = mode->height;
	m_windowConfig.Width = mode->width;
	m_windowConfig.Title = std::string("AIngine");
}

Game::~Game()
{
	DEBUG_WARN("Destructor Game");
}

void Game::OnAppStartUp()
{
	DEBUG_INFO("OnAppStartUp");

	// spawn ground
	AIngine::Assets::BitmapAsset* bitmap = GetAssetRegistry().Load<AIngine::Assets::BitmapAsset>(std::string("assets/Intellgine/textures/White.png"));
	AIngine::GameObject* ground = AIngine::World::SpawnObject();
	AIngine::Rendering::Texture2D* texture = ground->AddComponent<AIngine::Rendering::Texture2D>();
	texture->Generate(bitmap->GetBitmap());
	texture->SetLocalWorldSize(glm::vec2(20, 0.5));
	ground->SetLocalPosition(glm::vec2(10, 5));

	AIngine::PhysicsComponent* phys = ground->AddComponent<AIngine::PhysicsComponent>();

	b2BodyDef bodydef;
	bodydef.type = b2_staticBody;
	b2PolygonShape shape;
	shape.SetAsBox(10, 0.25);
	b2FixtureDef fixturedef;
	fixturedef.shape = &shape;
	fixturedef.density = 1.0;
	phys->CreateBody(bodydef, fixturedef)/*->SetTransform(b2Vec2(ground->GetWorldPosition().x, ground->GetWorldPosition().y), 0.0)*/;
}

void Game::OnAppShutDown()
{
	DEBUG_INFO("OnAppShutdown");
}

void Game::OnAppUpdate()
{
	/* camera movement */

	if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::A))
	{
		m_camera->Translate(glm::vec2(-translationrate, 0.0));
	}

	if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::D))
	{
		m_camera->Translate(glm::vec2(translationrate, 0.0));
	}


	if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::W))
	{
		m_camera->Translate(glm::vec2(0.0, -translationrate));
	}


	if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::S))
	{
		m_camera->Translate(glm::vec2(0.0, +translationrate));
	}

	if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::E))
	{
		m_camera->Rotate(rotationrate *  D2R * GetDeltaTime());
	}

	if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::Q))
	{
		m_camera->Rotate(-rotationrate * D2R * GetDeltaTime());
	}

	if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::F))
	{
		m_camera->Zoom(zoomSpeed * GetDeltaTime());
	}

	if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::G))
	{
		m_camera->Zoom(-zoomSpeed * GetDeltaTime());
	}

}

void Game::OnAppEvent(AIngine::Events::Event & e)
{
	if (typeid(e) == typeid(AIngine::Events::MouseButtonPressedEvent)) {
		// mouse pressed
		AIngine::Events::MouseButtonPressedEvent keyevent = dynamic_cast<AIngine::Events::MouseButtonPressedEvent&>(e);
		if (keyevent.GetMouseButton() == 0 && !IsAnyUiElementHovered()) {
			static std::string path("assets/Intellgine/textures/Circle.png");
			static AIngine::Assets::BitmapAsset* bitmapAsset = GetAssetRegistry().Load<AIngine::Assets::BitmapAsset>(path);
			static glm::vec2 minSize(0.1, 0.1);
			static glm::vec2 maxSize(2, 2);

			AIngine::GameObject* spawn = SpawnObjectAtMousePosition(minSize, maxSize, bitmapAsset->GetBitmap());

			AIngine::PhysicsComponent* phys = spawn->AddComponent<AIngine::PhysicsComponent>();
			b2BodyDef bodydef;
			bodydef.type = b2_dynamicBody;
			b2CircleShape shape;
			//shape.m_p = b2Vec2(spawn->GetWorldPosition().x, spawn->GetWorldPosition().y);
			shape.m_radius = spawn->GetComponent<AIngine::Rendering::Texture2D>()->GetLocalWorldSize().x;
			b2FixtureDef fixturedef;
			fixturedef.shape = &shape;
			fixturedef.density = 1.0;
			phys->CreateBody(bodydef, fixturedef);
			//body->SetTransform(b2Vec2(spawn->GetWorldPosition().x, spawn->GetWorldPosition().y), spawn->GetLocalRotation());
		}

		if (keyevent.GetMouseButton() == 1 && !IsAnyUiElementHovered()) {
			static std::string path("assets/Intellgine/textures/White.png");
			static AIngine::Assets::BitmapAsset* bitmapAsset = GetAssetRegistry().Load<AIngine::Assets::BitmapAsset>(path);
			static glm::vec2 minSize(0.1, 0.1);
			static glm::vec2 maxSize(2, 2);

			AIngine::GameObject* spawn = SpawnObjectAtMousePosition(minSize, maxSize, bitmapAsset->GetBitmap());

			AIngine::PhysicsComponent* phys = spawn->AddComponent<AIngine::PhysicsComponent>();
			b2BodyDef bodydef;
			bodydef.type = b2_dynamicBody;
			b2PolygonShape shape;
			shape.SetAsBox(0.5, 0.5);
			b2FixtureDef fixturedef;
			fixturedef.shape = &shape;
			fixturedef.density = 1.0;
			phys->CreateBody(bodydef, fixturedef);
			//body->SetTransform(b2Vec2(spawn->GetWorldPosition().x, spawn->GetWorldPosition().y), spawn->GetLocalRotation());
		}
	}
}

AIngine::GameObject* Game::SpawnObjectAtMousePosition(const glm::vec2 & minSize, const glm::vec2 & maxSize, Bitmap& bitmap)
{
	static int spawnCount = 0;

	glm::vec2 textureSize(1);
	//textureSize.x = minSize.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxSize.x));
	//textureSize.y = textureSize.x; /*minSize.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxSize.y));*/

	glm::vec2 spawnPosition(0);
	glm::vec2 mousePosition = glm::vec2(AIngine::Input::GetMousePosition().first, AIngine::Input::GetMousePosition().second);
	spawnPosition = m_camera->ScreenToWorldPoint(mousePosition);

	float rotation = 0.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / M_PI));

	std::string name = std::string("GameObject");
	if (spawnCount > 0) {
		name.append("(");
		name.append(std::to_string(spawnCount));
		name.append(")");
	}

	// color
	float red = 0.1 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float green = 0.1 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float blue = 0.1 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	AIngine::GameObject* obj = AIngine::World::SpawnObject(name, nullptr, spawnPosition, glm::vec2(1.0), rotation);

	spawnCount++;

	AIngine::Rendering::Texture2D* tex = obj->AddComponent<AIngine::Rendering::Texture2D>();
	tex->Generate(bitmap);
	tex->SetLocalWorldSize(textureSize);
	tex->SetColor(glm::vec3(red, green, blue));

	return obj;
}

