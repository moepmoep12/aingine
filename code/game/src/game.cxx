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
	AIngine::Assets::BitmapAsset* bitmap = GetAssetRegistry().Load<AIngine::Assets::BitmapAsset>(std::string("assets/Intellgine/textures/White.png"));
	AIngine::GameObject* ground = AIngine::World::SpawnObject();
	ground->SetName("Ground");
	AIngine::Rendering::Texture2D* texture = ground->AddComponent<AIngine::Rendering::Texture2D>();
	texture->Generate(bitmap->GetBitmap());
	texture->SetLocalWorldSize(glm::vec2(10, 1));
	ground->SetLocalPosition(glm::vec2(5, 5));

	AIngine::PhysicsComponent* phys = ground->AddComponent<AIngine::PhysicsComponent>();

	b2BodyDef bodydef;
	bodydef.type = b2_staticBody;
	b2PolygonShape shape;
	shape.SetAsBox(5, 0.5);
	b2FixtureDef fixturedef;
	fixturedef.shape = &shape;
	fixturedef.density = 1.0;
	phys->CreateBody(bodydef, fixturedef);
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
	if (typeid(e) == typeid(AIngine::Events::MouseScrolledEvent)) {
		AIngine::Events::MouseScrolledEvent scrolledEvent = dynamic_cast<AIngine::Events::MouseScrolledEvent&>(e);
		static float zoomSpeed = 10;
		m_camera->Zoom(scrolledEvent.GetYOffset() * GetDeltaTime() * zoomSpeed);
	}

	if (typeid(e) == typeid(AIngine::Events::MouseButtonPressedEvent)) {
		// mouse pressed
		AIngine::Events::MouseButtonPressedEvent keyevent = dynamic_cast<AIngine::Events::MouseButtonPressedEvent&>(e);

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
}


AIngine::GameObject * Game::SpawnObjectAtMousePosition(const glm::vec2 & minSize, const glm::vec2 & maxSize, Shape shape)
{
	static std::string circleImagePath("assets/Intellgine/textures/Circle.png");
	static std::string boxImagePath("assets/Intellgine/textures/White.png");
	static Bitmap* circleBitmap = &GetAssetRegistry().Load<AIngine::Assets::BitmapAsset>(circleImagePath)->GetBitmap();
	static Bitmap* boxBitmap = &GetAssetRegistry().Load<AIngine::Assets::BitmapAsset>(boxImagePath)->GetBitmap();
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
		Texture2D* texture = spawnedObject->AddComponent<Texture2D>();
		texture->Generate(shape == e_Circle ? *circleBitmap : *boxBitmap);
		texture->SetLocalWorldSize(worldSize);
		texture->SetColor(color);
	}

	// add physicsComponent


	AIngine::PhysicsComponent* physComponent = spawnedObject->AddComponent<AIngine::PhysicsComponent>();
	static b2CircleShape circleShape;
	static b2PolygonShape polyShape;

	b2BodyDef bodydef;
	bodydef.type = b2_dynamicBody;

	b2FixtureDef fixturedef;
	fixturedef.density = 1.0f;

	if (shape == e_Circle) {
		circleShape.m_radius = worldSize.x / 2.0f;
		fixturedef.shape = &circleShape;
	}
	else {
		polyShape.SetAsBox(worldSize.x / 2.0f, worldSize.y / 2.0f);
		fixturedef.shape = &polyShape;
	}

	physComponent->CreateBody(bodydef, fixturedef);

	return spawnedObject;

}

