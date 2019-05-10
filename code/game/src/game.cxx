#pragma once
#include "game.h"
#include "Layer.h"
#include "Core.h"
#include "Rendering/texture.h"
#include <glm/glm.hpp>
#include <random>

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
		if (ImGui::Begin("Camera"))
		{
			AIngine::Application& app = AIngine::Application::Get();
			AIngine::Rendering::Camera& cam = app.GetCamera();

			// cam settings
			static float dragspeed = 0.1f;
			float camzoom = cam.GetZoom();
			ImGui::DragFloat("Zoom", &camzoom, dragspeed, cam.GetZoomMin(), cam.GetZoomMax());
			ImGui::DragFloat("MoveSpeed", &translationrate, dragspeed, 0.01f, 1000.0f);
			ImGui::DragFloat("RotationSpeed", &rotationrate, dragspeed, 0.01f, 1000.0f);
			cam.SetZoom(camzoom);

			float pos[] = { cam.GetPosition().x, cam.GetPosition().y };
			ImGui::DragFloat2("Cam Position", pos);

			// mouse positions
			glm::vec2 mouseScreenPos = glm::vec2(AIngine::Input::GetMouseX(), AIngine::Input::GetMouseY());
			glm::vec2 mouseWorldPos = cam.ScreenToWorldPoint(mouseScreenPos);
			glm::vec2 screentopoint = app.GetViewport().PointToScreen(mouseScreenPos);
			ImGui::Text("Mouse screen Pos (%.1f | %.1f)", mouseScreenPos.x, mouseScreenPos.y);
			ImGui::Text("ScreenToPoint (%.1f | %.1f)", screentopoint.x, screentopoint.y);
			ImGui::Text("Mouse world Pos (%.1f | %.1f)", mouseWorldPos.x, mouseWorldPos.y);


			// reset camera button
			if (ImGui::Button("Reset")) {
				cam.SetPosition(glm::vec2(0));
				cam.SetZoom(1.0);
				cam.SetRotation(0.0);
			}

			static glm::vec2 lookpos(0);
			static float* lookposvalues[] = { &lookpos.x, &lookpos.y };

			ImGui::DragFloat2("LookPositionX", *lookposvalues);
			if (ImGui::Button("Look At")) {
				cam.LookAt(lookpos);
			}

			ImGui::End();
		}
	}


	~ExampleLayer() {
		DEBUG_INFO("Destructor ExampleLayer");
	}

};

Game::Game()
{
	DEBUG_WARN("Creating Game...");
	PushLayer(new ExampleLayer());
	m_gravity = (b2Vec2(0.0, -10.0));
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

		//AIngine::GameObject* obj = m_sceneGraph->SpawnObject();
		//AIngine::Rendering::Texture2D* texture = obj->AddComponent<AIngine::Rendering::Texture2D>();
		//AIngine::Assets::BitmapAsset* bitmap = app.GetAssetRegistry().Load<AIngine::Assets::BitmapAsset>(path);
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
	Application& app = AIngine::Application::Get();

	// spawning sprites with random shapes
	if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::SPACE)) {
		int width = app.GetWindow().GetWidth();
		int height = app.GetWindow().GetHeight();
		static float maxSizeX = 700;
		static float maxSizeY = 700;
		static float minSizeX = 200;
		static float minSizeY = 200;
		static float minRot = -M_PI;
		static float maxRot = M_PI;
		static glm::vec2 minWorld(0, 0);
		static glm::vec2 maxWorld(2000, 1000);
		static std::string path("assets/game/textures/awesomeface.png");


		AIngine::GameObject* obj = m_sceneGraph->SpawnObject();
		AIngine::Rendering::Texture2D* texture = obj->AddComponent<AIngine::Rendering::Texture2D>();
		AIngine::Assets::BitmapAsset* bitmap = app.GetAssetRegistry().Load<AIngine::Assets::BitmapAsset>(path);
		texture->Generate(bitmap->GetBitmap());

		// sprite size
		float sizeX = minSizeX + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxSizeX));
		float sizeY = minSizeY + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxSizeY));
		texture->SetLocalWorldSize(glm::vec2(sizeX, sizeY));

		// position
		//glm::vec2 textureSize = glm::vec2(obj->GetLocalScale().x * texture->Width, obj->GetLocalScale().y * texture->Height);
		float posX = minWorld.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxWorld.x));
		float posY = minWorld.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxWorld.y));
		obj->SetLocalPosition(glm::vec2(posX, posY));

		// rotation
		float rot = minRot + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxRot));
		obj->SetRotation(rot);

		// color
		float red = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float green = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float blue = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		texture->SetColor(glm::vec3(red, green, blue));
	}



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
		m_camera->Translate(glm::vec2(0.0, translationrate));
	}


	if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::S))
	{
		m_camera->Translate(glm::vec2(0.0, -translationrate));
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

