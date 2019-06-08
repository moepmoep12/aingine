#pragma once
#include "game.h"
#include "Structures/Layer.h"
#include "Rendering/texture.h"
#include <glm/glm.hpp>
#include <random>
#include "imgui.h"
#include "Events/InputEvents.h"

AIngine::Application* AIngine::CreateApplication() {
	return new Game();
}

// cam settings
static float translationrate = 5.0f;
static float rotationrate = 0.1f;
static float zoomSpeed = 5.0f;


class ExampleLayer :public  AIngine::Structures::Layer {


public:
	ExampleLayer() : Layer("ExampleLayer") {

	}

	virtual void OnEvent(AIngine::Events::EventData& e) override
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

	// configure game world
	m_gravity = (glm::vec2(0.0, 5.0));
	m_bounds = glm::vec4(0.0, 10.0, 0.0, 10.0);

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
	AIngine::Assets::BitmapAsset* bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(std::string("assets/Intellgine/textures/White.png"));
	AIngine::GameObject* ground = AIngine::World::SpawnObject();
	ground->SetName("Ground");
	AIngine::Sprite* sprite = ground->AddComponent<AIngine::Sprite>();
	sprite->GetTexture().Generate(bitmap->GetBitmap());
	sprite->SetLocalWorldSize(glm::vec2(10, 1));
	ground->SetLocalPosition(glm::vec2(5, 5));

	AIngine::Physics::PhysicsComponent* phys = ground->AddComponent<AIngine::Physics::PhysicsComponent>();
	AIngine::Physics::PhysicsProperties properties;
	properties.density = 1.0f;
	phys->CreateBoxBody(properties, AIngine::Physics::PhysicsBodyType::e_Static, 10, 1);
	////phys->CreateBoxBody()
	//b2BodyDef bodydef;
	//bodydef.type = b2_staticBody;
	//b2PolygonShape shape;
	//shape.SetAsBox(5, 0.5);
	//b2FixtureDef fixturedef;
	//fixturedef.shape = &shape;
	//fixturedef.density = 1.0;
	//phys->CreateBody(bodydef, fixturedef);
}

void Game::OnAppShutDown()
{
	DEBUG_INFO("OnAppShutdown");
}

void Game::OnAppUpdate()
{
	/* camera movement */
#ifndef _DEBUG
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
		m_camera->Rotate(rotationrate *  AIngine::D2R * GetDeltaTime());
	}

	if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::Q))
	{
		m_camera->Rotate(-rotationrate * AIngine::D2R * GetDeltaTime());
	}
#endif // !_DEBUG

}

void Game::OnAppEvent(AIngine::Events::EventData & e)
{
#ifndef _DEBUG
	if (typeid(e) == typeid(AIngine::Events::MouseScrolledEvent::MouseScrolledEventData)) {
		if (!IsAnyUiElementHovered()) {
			AIngine::Events::MouseScrolledEvent::MouseScrolledEventData scrolledEvent = dynamic_cast<AIngine::Events::MouseScrolledEvent::MouseScrolledEventData&>(e);
			static float zoomSpeed = 35;
			m_camera->Zoom(scrolledEvent.GetYOffset() * GetDeltaTime() * zoomSpeed);
		}
	}
#endif // !_DEBUG


	if (typeid(e) == typeid(AIngine::Events::MouseButtonPressedEvent::MouseButtonPressedEventData)) {
		// mouse pressed
		AIngine::Events::MouseButtonPressedEvent::MouseButtonPressedEventData keyevent = dynamic_cast<AIngine::Events::MouseButtonPressedEvent::MouseButtonPressedEventData&>(e);

		if (keyevent.GetMouseButton() == 0 && !IsAnyUiElementHovered() && AIngine::Input::IsKeyPressed(AIngine::KeyCodes::LEFT_CONTROL))
		{
			//m_world->GetPhysicsWorld().ray
		}

		if (keyevent.GetMouseButton() == 0 && !IsAnyUiElementHovered()) {
			static glm::vec2 minSize(0.05, 0.05);
			static glm::vec2 maxSize(1, 1);

			AIngine::GameObject* spawn = SpawnObjectAtMousePosition(minSize, maxSize, e_Circle);
		}

		if (keyevent.GetMouseButton() == 1 && !IsAnyUiElementHovered()) {
			static glm::vec2 minSize(0.05, 0.05);
			static glm::vec2 maxSize(1, 1);

			AIngine::GameObject* spawn = SpawnObjectAtMousePosition(minSize, maxSize, e_Box);

		}
	}

	if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent::KeyPressedEventData)) {

		AIngine::Events::KeyPressedEvent::KeyPressedEventData keyEvent = dynamic_cast<AIngine::Events::KeyPressedEvent::KeyPressedEventData&>(e);

		if (keyEvent.GetKeyCode() == AIngine::KeyCodes::F) {
			glm::vec2 screenPos = glm::vec2(AIngine::Input::GetMouseX(), AIngine::Input::GetMouseY());
			m_camera->LookAt(/*m_camera->ScreenToWorldPoint(screenPos)*/glm::vec2(5, 5));
		}
	}
}


AIngine::GameObject * Game::SpawnObjectAtMousePosition(const glm::vec2 & minSize, const glm::vec2 & maxSize, Shape shape)
{
	static std::string circleImagePath("assets/Intellgine/textures/Circle.png");
	static std::string boxImagePath("assets/Intellgine/textures/White.png");
	static AIngine::Rendering::Bitmap* circleBitmap = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(circleImagePath)->GetBitmap();
	static AIngine::Rendering::Bitmap* boxBitmap = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(boxImagePath)->GetBitmap();
	static int circleCount = 0;
	static int boxCount = 0;


	// spawnPosition
	glm::vec2 spawnPosition(0);
	glm::vec2 mousePosition = glm::vec2(AIngine::Input::GetMousePosition().first, AIngine::Input::GetMousePosition().second);
	spawnPosition = m_camera->ScreenToWorldPoint(mousePosition);

	// rotation
	float rotation = 0.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / M_PI));

	// color
	glm::vec3 color(1.0);
	color.x = 0.1f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	color.y = 0.1f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	color.z = 0.1f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	// worldSize
	glm::vec2 worldSize(1);
	worldSize.x = minSize.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxSize.x));
	worldSize.y = shape == e_Circle ? worldSize.x : minSize.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxSize.y));

	// name
	std::string name;
	if (shape == e_Circle) {
		name = std::string("Circle");
		if (circleCount > 0) {
			name.append("(");
			name.append(std::to_string(circleCount));
			name.append(")");
		}
		circleCount++;
	}
	else if (shape == e_Box) {
		name = std::string("Box");
		if (boxCount > 0) {
			name.append("(");
			name.append(std::to_string(boxCount));
			name.append(")");
		}
		boxCount++;
	}

	// spawn the object
	AIngine::GameObject* spawnedObject = AIngine::World::SpawnObject(name, nullptr, spawnPosition, glm::vec2(1.0), rotation);

	// add texture
	{
		using namespace AIngine::Rendering;
		AIngine::Sprite* sprite = spawnedObject->AddComponent<AIngine::Sprite>();
		Texture2D& texture = sprite->GetTexture();
		texture.Generate(shape == e_Circle ? *circleBitmap : *boxBitmap);
		sprite->SetLocalWorldSize(worldSize);
		sprite->SetColor(color);
	}

	// add physicsComponent

	AIngine::Physics::PhysicsComponent* physComponent = spawnedObject->AddComponent<AIngine::Physics::PhysicsComponent>();
	if (shape == e_Circle) {
		float32 radius = worldSize.x / 2.0f;
		AIngine::Physics::PhysicsProperties properties;
		properties.density = 1.0f;
		physComponent->CreateCircleBody(properties, AIngine::Physics::PhysicsBodyType::e_Dynamic, radius);
	}
	else {
		float width = worldSize.x;
		float height = worldSize.y;
		AIngine::Physics::PhysicsProperties properties;
		properties.density = 1.0f;
		physComponent->CreateBoxBody(properties, AIngine::Physics::PhysicsBodyType::e_Dynamic, width, height);
	}



	return spawnedObject;

}

