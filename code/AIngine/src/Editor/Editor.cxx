#include "Editor/Editor.h"
#include "AIngine/Macros.h"
#include "Application.h"
#include "AIngine/KeyCodes.h"
#include "AIngine/Input.h"
#include "AIngine/Constants.h"
#include "Structures/SceneGraph.h"
#include "Editor/Serialization.h"
#include "Debug/log.h"

// widgets
#include "Editor/Widgets/EditorWidget.h"
#include "Editor/Widgets/SceneGraphWidget.h"
#include "Editor/Widgets/LogWidget.h"
#include "Editor/Widgets/CameraWidget.h"
#include "Editor/Widgets/MenuBarWidget.h"
#include "Editor/Widgets/ToolbarWidget.h"

#include <glm/geometric.hpp>
#include <fstream>
#include <filesystem>
#include <nfd.h>

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
	static float zoomSpeed = 100;

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

	// the path to the file that contains the path to the scene that was last opened
	static const std::string lastSceneFilePath = "scenes.txt";

	void Editor::OnWindowClose()
	{
		// remember the path to the scene
		std::ofstream file(lastSceneFilePath);
		file << m_currentSceneFilePath;
		file.close();
	}


	void Editor::LoadLastScene()
	{
		// retrieve the path to the scene that was last opened
		std::ifstream file;
		file.open(lastSceneFilePath);
		std::getline(file, m_currentSceneFilePath);
		file.close();

		if (!m_currentSceneFilePath.empty()) {
			// save the path as relative path
			m_currentSceneFilePath = std::filesystem::relative(m_currentSceneFilePath).string();
			// load the scene
			AIngine::Editor::Serialization::Serializer::DeserializeSceneGraph(m_currentSceneFilePath);

			UpdateSceneTitle();
		}
	}

	void Editor::SaveOpenScene()
	{
		// remember the path to the scene
		std::ofstream file(lastSceneFilePath);
		file << m_currentSceneFilePath;
		file.close();

		AIngine::Editor::Serialization::Serializer::SerializeSceneGraph(m_currentSceneFilePath);
	}

	std::string Editor::UpdateSceneTitle()
	{
		std::string Path(std::filesystem::canonical(m_currentSceneFilePath).string());
		unsigned int first = Path.find_last_of('\\') + 1;
		unsigned int last = Path.find_last_of('.');
		std::string sceneName = " | ";
		sceneName.append(Path.substr(first, last - first).c_str());
		m_app.m_window->AppendWindowTitle(sceneName.c_str());
		return Path.substr(first, last - first).c_str();
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
		m_widgets.push_back(new ToolbarWidget());
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

					//if (widgetPos.x > viewportPos.x
					//	&& widgetPos.y == viewportPos.y
					//	&& widgetMax.x == viewportMax.x
					//	&& widgetMax.y == viewportMax.y)
					//{
					//	viewportRect.width -= widgetRect.width;
					//}
					//if (widgetPos.x == viewportPos.x
					//	&& widgetPos.y == viewportPos.y
					//	&& widgetMax.x < viewportMax.x
					//	&& widgetMax.y == viewportMax.y)
					//{
					//	viewportRect.x += widgetRect.width;
//}
//if (widgetPos.x == viewportPos.x
//	&& widgetPos.y > viewportPos.y
//	&& widgetMax.x == viewportMax.x
//	&& widgetMax.y == viewportMax.y)
//{
//	viewportRect.height -= widgetRect.height;
//}
//if (widgetPos.x == viewportPos.x
//	&& widgetPos.y == viewportPos.y
//	&& widgetMax.x == viewportMax.x
//	&& widgetMax.y < viewportMax.y)
//{
//	viewportRect.y += widgetRect.height;
//}

					if (widgetPos.x > viewportPos.x) {
						viewportRect.width = widgetPos.x - viewportPos.x;
					}
					else if (widgetPos.x < viewportPos.x) {
						viewportRect.x = widgetPos.x + widgetMax.x;
					}

					if (widgetPos.y > viewportPos.y) {
						viewportRect.height = widgetPos.y - viewportPos.y;
					}
					else if (widgetPos.y < viewportPos.y) {
						viewportRect.y = widgetPos.y + widgetMax.y;
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
		m_app.m_world->GetSceneGraph().Reset();
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

	bool Editor::CreateMoveablePositionVertex(glm::vec2 & worldPosition, float vertexSize, const glm::vec3 & colorInteract, const glm::vec3 & colorNormal)
	{
		glm::vec2 mouseScreenPos = glm::vec2(AIngine::Input::GetMousePosition().first, AIngine::Input::GetMousePosition().second);
		glm::vec2 mouseWorldPos = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(mouseScreenPos);
		glm::vec2 screenPos = AIngine::Rendering::Camera::Get().WorldToScreenPoint(worldPosition);
		AIngine::Structures::Rectangle vertexRectangle(screenPos.x - vertexSize, screenPos.y - vertexSize, 2 * vertexSize, 2 * vertexSize);
		bool bInteracted = false;

		if (vertexRectangle.Contains(mouseScreenPos)) {
			if (AIngine::Input::IsMouseButtonPressed(0)) {
				glm::vec2 diff = mouseWorldPos - worldPosition;
				worldPosition += diff;
				screenPos = AIngine::Rendering::Camera::Get().WorldToScreenPoint(worldPosition);
				vertexRectangle = AIngine::Structures::Rectangle(screenPos.x - vertexSize, screenPos.y - vertexSize, 2 * vertexSize, 2 * vertexSize);
				bInteracted = true;
			}
			AIngine::Graphics::Point(worldPosition, vertexSize, colorInteract);
		}
		else {
			AIngine::Graphics::Point(worldPosition, vertexSize, colorNormal);
		}

		AIngine::Graphics::BoxScreen(vertexRectangle, glm::vec3(1, 1, 0));
		return bInteracted;
	}

	void Editor::CreateNewScene()
	{
		if (s_instance) {
			s_instance->m_currentSceneFilePath = "";
			s_instance->ResetSceneGraph();
		}
	}

	void Editor::LoadScene(const std::string& path)
	{
		if (s_instance) {
			// unload the currently open scene
			s_instance->ResetSceneGraph();
			// load the new scene
			AIngine::Editor::Serialization::Serializer::DeserializeSceneGraph(path);
			// remember the path to the new scene
			s_instance->m_currentSceneFilePath = std::filesystem::relative(path).string();

			std::string sceneName =  s_instance->UpdateSceneTitle();

			CORE_INFO("Loaded ccene " + sceneName + " from " + path);
		}
	}

	void Editor::SaveScene(const std::string& path)
	{
		if (s_instance) {
			AIngine::Editor::Serialization::Serializer::SerializeSceneGraph(path);
			s_instance->m_currentSceneFilePath = std::filesystem::relative(path).string();
			CORE_INFO("Saved scene to " + path);
		}
	}

	void Editor::LoadSceneFromFile()
	{
		static const nfdchar_t *filterList = "txt,json";
		nfdchar_t *outPath = NULL;
		nfdresult_t result = NFD_OpenDialog(filterList, NULL, &outPath);

		if (result == NFD_OKAY)
		{
			LoadScene(outPath);
			free(outPath);
		}
	}

	bool Editor::SaveSceneToFile()
	{
		static const nfdchar_t *filterList = "txt,json";
		nfdchar_t *outPath = NULL;
		nfdresult_t result = NFD_SaveDialog(filterList, NULL, &outPath);

		if (result == NFD_OKAY)
		{
			SaveScene(outPath);
			free(outPath);
			return true;
		}
		return false;
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
				s_instance->OnEnterPlayModeEvent();
			}
			else {
				s_instance->OnLeavePlayModeEvent();
				s_instance->ResetSceneGraph();
				// load the scene
				s_instance->LoadLastScene();
			}
			s_instance->m_isInPlayMode = value;

		}
	}

}

// initialize ViewportChangedEventHandler counter
int AIngine::Events::EventHandler<void, AIngine::Structures::Rectangle&>::counter = 0;