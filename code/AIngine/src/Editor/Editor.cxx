#include "Editor/Editor.h"
#include "AIngine/Macros.h"
#include "Application.h"
#include "AIngine/KeyCodes.h"
#include "AIngine/Input.h"
#include "AIngine/Constants.h"
#include "Structures/SceneGraph.h"
#include "Editor/Serialization.h"

// widgets
#include "Editor/Widgets/EditorWidget.h"
#include "Editor/Widgets/SceneGraphWidget.h"
#include "Editor/Widgets/LogWidget.h"
#include "Editor/Widgets/CameraWidget.h"
#include "Editor/Widgets/MenuBarWidget.h"

#include <glm/geometric.hpp>
#include <fstream>

namespace AIngine::Editor {

	Editor* Editor::Editor::s_instance = nullptr;

	void Editor::OnAttach()
	{
		//activate widgets?
		m_isInPlayMode = false;
	}

	void Editor::OnDetach()
	{
		m_isInPlayMode = true;
	}

	void Editor::OnUpdate(float delta)
	{
		if (DidAnyDockedWidgetChangeSize()) {
			AIngine::Structures::Rectangle viewportRect = CalculateViewportRect(glm::vec2(m_app.GetWindow().GetWidth(), m_app.GetWindow().GetHeight()));
			OnViewportChangedEvent(viewportRect);
		}

		MoveCamera(delta);
		if (m_displayingFramerate) DisplayFramerate(delta);
	}

	void Editor::OnEvent(AIngine::Events::EventData & e)
	{
		AIngine::Events::EventDispatcher dispatcher(e);

		// call the OnWindowClose function if its a windowclose event
		dispatcher.Dispatch<AIngine::Events::KeyPressedEvent::KeyPressedEventData>(BIND_EVENT_TO_FN(Editor::OnKeyPressed));
		dispatcher.Dispatch<AIngine::Events::MouseScrolledEvent::MouseScrolledEventData>(BIND_EVENT_TO_FN(Editor::OnMouseScrolled));

		auto it = m_widgets.begin();
		while (it != m_widgets.end()) {
			(*it._Ptr)->OnEvent(e);
			it++;
		}

	}

	void Editor::OnImGuiRender()
	{

		auto it = m_widgets.begin();

		while (it != m_widgets.end()) {
			(*it._Ptr)->OnImGuiRender();
			it++;
		}
	}

	bool Editor::OnKeyPressed(AIngine::Events::KeyPressedEvent::KeyPressedEventData & e)
	{
		// Toggle PhysicsDebugDraw
		if (e.GetKeyCode() == AIngine::KeyCodes::F1)
		{
			AIngine::World::SetPhysicsDebugDrawActive(!AIngine::World::s_instance->m_isPhysicsDebugDrawn);
			return true;
		}

		if (e.GetKeyCode() == AIngine::KeyCodes::F2)
		{
			m_displayingFramerate = !m_displayingFramerate;
		}

		return false;
	}

	bool Editor::OnWindowResized(AIngine::Events::WindowResizeEvent::WindowResizeEventData & e)
	{
		AIngine::Structures::Rectangle viewportRect = CalculateViewportRect(glm::vec2(m_app.GetWindow().GetWidth(), m_app.GetWindow().GetHeight()));
		OnViewportChangedEvent(viewportRect);
		return true;
	}

	// cam settings
	static float translationrate = 5.0f;
	static float rotationrate = 0.1f;
	static float zoomSpeed = 35;

	bool Editor::OnMouseScrolled(AIngine::Events::MouseScrolledEvent::MouseScrolledEventData & e)
	{
		if (!m_app.IsAnyUiElementHovered()) {
			m_app.m_camera->Zoom(e.GetYOffset() * m_app.GetDeltaTime() * zoomSpeed);
		}
		return true;
	}

	void Editor::MoveCamera(float delta)
	{
		AIngine::Rendering::Camera* camera = m_app.m_camera;

		if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::A))
		{
			camera->Translate(glm::vec2(-translationrate, 0.0));
		}

		if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::D))
		{
			camera->Translate(glm::vec2(translationrate, 0.0));
		}


		if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::W))
		{
			camera->Translate(glm::vec2(0.0, -translationrate));
		}


		if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::S))
		{
			camera->Translate(glm::vec2(0.0, +translationrate));
		}

		if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::E))
		{
			camera->Rotate(rotationrate *  AIngine::D2R * delta);
		}

		if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::Q))
		{
			camera->Rotate(-rotationrate * AIngine::D2R * delta);
		}
	}

	void Editor::OnWindowClose()
	{
		//SaveOpenScene();
	}

	static const std::string lastSceneFilePath = "scenes.txt";

	void Editor::LoadLastScene()
	{
		std::ifstream file;
		file.open(lastSceneFilePath);
		std::getline(file, m_currentScene);
		if (!(m_currentScene.empty())) {
			AIngine::Editor::Serialization::Serializer::DeserializeSceneGraph(m_currentScene);
		}
		file.close();
	}

	void Editor::SaveOpenScene()
	{
		std::ofstream file(lastSceneFilePath);
		file << m_currentScene;
		AIngine::Editor::Serialization::Serializer::SerializeSceneGraph(m_currentScene);
		file.close();
	}

	Editor::Editor()
		: m_app(AIngine::Application::Get())
	{
		ASSERT(!s_instance, "Editor already running");
		s_instance = this;

		// register callback
		m_app.m_window->GetWindowData().OnWindowClosedEvent += [=]() {
			this->OnWindowClose();
		};

		// load the last opened scene
		LoadLastScene();

		// Create Editor Widgets
		m_widgets.push_back(new SceneGraphWidget(*m_app.m_world->m_sceneGraph));
		m_widgets.push_back(new LogWidget());
		m_widgets.push_back(new CameraWidget(*m_app.m_camera));
		m_widgets.push_back(new MenubarWidget());
	}

	AIngine::Structures::Rectangle Editor::CalculateViewportRect(const glm::vec2& windowSize) const
	{
		using Rectangle = AIngine::Structures::Rectangle;
		using Corner = AIngine::Structures::Rectangle::Corner;

		Rectangle viewportRect(0, m_widgets[3]->GetSize().y, windowSize.x, windowSize.y - m_widgets[3]->GetSize().y);

		auto it = m_widgets.begin();

		while (it != m_widgets.end()) {
			const EditorWidget& widget = *(*it._Ptr);
			if (widget.IsWindowDocked()) {
				AIngine::Structures::Rectangle widgetRect = widget.GetRectangle();
				if (viewportRect.Contains(widgetRect)) {
					glm::vec2 windowPos = glm::vec2(m_app.m_window->GetX(), m_app.m_window->GetY());
					glm::vec2 viewportPos = viewportRect.GetPosition();
					glm::vec2 viewportMax = viewportRect.GetMax();
					glm::vec2 widgetPos = widgetRect.GetPosition() - windowPos /*- m_widgets[3]->GetPosition().y - m_widgets[3]->GetSize().y*/;
					glm::vec2 widgetMax = widgetRect.GetMax() - windowPos/* - m_widgets[3]->GetPosition().y - m_widgets[3]->GetSize().y*/;

					if (widgetPos.x > viewportPos.x
						&& widgetPos.y == viewportPos.y
						&& widgetMax.x == viewportMax.x
						&& widgetMax.y == viewportMax.y)
					{
						viewportRect.width -= widgetRect.width;
					}
					if (widgetPos.x == viewportPos.x
						&& widgetPos.y == viewportPos.y
						&& widgetMax.x < viewportMax.x
						&& widgetMax.y == viewportMax.y)
					{
						viewportRect.x += widgetRect.width;
					}
					if (widgetPos.x == viewportPos.x
						&& widgetPos.y > viewportPos.y
						&& widgetMax.x == viewportMax.x
						&& widgetMax.y == viewportMax.y)
					{
						viewportRect.height -= widgetRect.height;
					}
					if (widgetPos.x == viewportPos.x
						&& widgetPos.y == viewportPos.y
						&& widgetMax.x == viewportMax.x
						&& widgetMax.y < viewportMax.y)
					{
						viewportRect.y += widgetRect.height;
					}
				}
			}
			it++;
		}

		return viewportRect;
	}

	void Editor::SetShowFramerate(bool active)
	{
		if (s_instance)
			s_instance->m_displayingFramerate = active;
	}

	void Editor::ResetSceneGraph()
	{
		if (s_instance)
			s_instance->GetSceneGraph()->Reset();
	}

	AIngine::Structures::SceneGraph * Editor::GetSceneGraph()
	{
		if (s_instance) {
			return &s_instance->m_app.m_world->GetSceneGraph();
		}
		return nullptr;
	}

	bool Editor::IsAnyUIElementHovered()
	{
		return ImGui::IsAnyWindowHovered();
	}

	void Editor::DisplayFramerate(float delta) const
	{
		std::stringstream ss;
		ss << "FPS: " << (int)(1.0f / delta);
		AIngine::Graphics::Text(ss.str().c_str(), glm::vec2(0, 0), glm::vec2(0.75));
	}

	bool Editor::DidAnyDockedWidgetChangeSize() const
	{
		auto it = m_widgets.begin();

		while (it != m_widgets.end()) {
			const EditorWidget& widget = *(*it._Ptr);
			if (widget.WasWindowSizeChanged() && widget.IsWindowDocked())
				return true;
			it++;
		}
		return false;
	}

	Editor::~Editor()
	{
		s_instance = nullptr;

		for (unsigned int i = 0; i < m_widgets.size(); i++) {
			delete m_widgets[i];
		}
		m_widgets.clear();
	}

	void Editor::SetIsInPlayMode(bool value)
	{
		if (s_instance) {
			if (value) {
				// save the scene
				s_instance->SaveOpenScene();
				s_instance->m_app.OnAppStartUp();
			}
			else {
				s_instance->m_app.OnAppShutDown();
				ResetSceneGraph();
				// load the scene
				s_instance->LoadLastScene();
			}
			s_instance->m_isInPlayMode = value;

		}
	}

}

// initialize ViewportChangedEventHandler counter
int AIngine::Events::EventHandler<void, AIngine::Structures::Rectangle&>::counter = 0;