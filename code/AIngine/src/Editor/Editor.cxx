#include "Editor/Editor.h"
#include "AIngine/Macros.h"
#include "Application.h"
#include "AIngine/Constants.h"
#include "AIngine/Input.h"
#include "AIngine/Graphics.h"
#include "AIngine/KeyCodes.h"
#include "AIngine/World.h"
#include "Structures/SceneGraph.h"
#include "Editor/Serialization.h"
#include "Rendering/Viewport.h"
#include "Debug/log.h"
#include "Util/Project.h"
#include "Util/FileSystem.h"
#include "Util/SystemCommand.h"

// widgets
#include "Editor/Widgets/EditorWidget.h"
#include "Editor/Widgets/SceneGraphWidget.h"
#include "Editor/Widgets/LogWidget.h"
#include "Editor/Widgets/CameraWidget.h"
#include "Editor/Widgets/MenuBarWidget.h"
#include "Editor/Widgets/ToolbarWidget.h"

#include <glm/geometric.hpp>
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace AIngine::Editor {

	Editor* Editor::Editor::s_instance = nullptr;
	AIngine::Events::Event<void> Editor::PauseGameEvent;
	AIngine::Events::Event<void> Editor::ResumeGameEvent;
	AIngine::Events::Event<void,const std::vector<GameObject*>&> Editor::OnSelectionChangedEvent;
	static glm::vec2 lastMousePos;


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
		delta = m_app.GetDeltaTimeReal();
		if (m_hasViewportFocus) MoveCameraWithKeys(delta);
		if (m_isDraggingCamera) MoveCameraWithMouse(delta);
	}

	void Editor::OnEvent(AIngine::Events::EventData & e)
	{
		AIngine::Events::EventDispatcher dispatcher(e);

		dispatcher.Dispatch<AIngine::Events::KeyPressedEvent::KeyPressedEventData>(BIND_EVENT_TO_FN(Editor::OnKeyPressed));
		dispatcher.Dispatch<AIngine::Events::MouseScrolledEvent::MouseScrolledEventData>(BIND_EVENT_TO_FN(Editor::OnMouseScrolled));
		dispatcher.Dispatch<AIngine::Events::MouseButtonPressedEvent::MouseButtonPressedEventData>(BIND_EVENT_TO_FN(Editor::OnMouseButtonPressed));
		dispatcher.Dispatch<AIngine::Events::MouseButtonReleasedEvent::MouseButtonReleasedEventData>(BIND_EVENT_TO_FN(Editor::OnMouseButtonReleased));

		auto it = m_widgets.begin();
		while (it != m_widgets.end()) {
			(*it._Ptr)->OnEvent(e);
			it++;
		}

	}

	void Editor::OnImGuiRender()
	{
		// don't draw widgets if in fullscreen mode
		if (!m_isFullScreen) {
			auto it = m_widgets.begin();

			while (it != m_widgets.end()) {
				(*it._Ptr)->OnImGuiRender();
				it++;
			}

			if (m_showingFpsGraph) DrawFpsGraph(m_app.GetDeltaTimeReal());
			if (m_displayingFramerate) DisplayFramerate(m_app.GetDeltaTimeReal());
		}
	}

	bool Editor::OnKeyPressed(AIngine::Events::KeyPressedEvent::KeyPressedEventData & e)
	{
		// Toggle PhysicsDebugDraw
		if (e.GetKeyCode() == AIngine::KeyCode::F1)
		{
			AIngine::World::SetPhysicsDebugDrawActive(!AIngine::World::s_instance->m_isPhysicsDebugDrawn);
			return true;
		}

		if (e.GetKeyCode() == AIngine::KeyCode::F2)
		{
			m_displayingFramerate = !m_displayingFramerate;
		}

		if (e.GetKeyCode() == AIngine::KeyCode::F4)
		{
			SetFullScreenPlayMode(!m_isFullScreen);
		}

		if (e.GetKeyCode() == AIngine::KeyCode::F5)
		{
			SetIsInPlayMode(!m_isInPlayMode);
		}

		return false;
	}

	bool Editor::OnWindowResized(AIngine::Events::WindowResizeEvent::WindowResizeEventData & e)
	{
		AIngine::Structures::RectangleI viewportRect;
		if (m_isFullScreen)
			viewportRect = AIngine::Structures::RectangleI(m_app.GetWindow().GetX(), m_app.GetWindow().GetY(), e.GetWidth(), e.GetHeight());
		else
			viewportRect = CalculateViewportRect(glm::vec2(m_app.GetWindow().GetWidth(), m_app.GetWindow().GetHeight()));

		OnViewportChangedEvent(viewportRect);

		return true;
	}

	// cam settings
	static float translationrate = 5.0f;
	static float rotationrate = 0.1f;
	static float zoomSpeed = 100;

	bool Editor::OnMouseScrolled(AIngine::Events::MouseScrolledEvent::MouseScrolledEventData & e)
	{
		if (!IsAnyUIElementHovered()) {
			m_app.m_camera->Zoom(e.GetYOffset() * m_app.GetDeltaTime() * zoomSpeed);
		}
		return true;
	}

	bool Editor::OnMouseButtonPressed(AIngine::Events::MouseButtonPressedEvent::MouseButtonPressedEventData & e)
	{
		glm::vec2 mousePos = glm::vec2(AIngine::Input::GetMousePosition().first, AIngine::Input::GetMousePosition().first);
		m_hasViewportFocus = m_app.GetViewport().Contains(mousePos);

		if (e.GetMouseButton() == MouseButton::BUTTON_MIDDLE) {
			if (m_hasViewportFocus) {
				lastMousePos = glm::vec2(Input::GetMouseX(), Input::GetMouseY());
				m_isDraggingCamera = true;
			}
		}
		return false;
	}

	bool Editor::OnMouseButtonReleased(AIngine::Events::MouseButtonReleasedEvent::MouseButtonReleasedEventData & e)
	{
		m_isDraggingCamera = false;
		return false;
	}

	void Editor::MoveCameraWithKeys(float delta)
	{
		AIngine::Rendering::Camera* camera = m_app.m_camera;

		if (AIngine::Input::IsKeyPressed(AIngine::KeyCode::A))
		{
			camera->Translate(glm::vec2(-translationrate, 0.0));
		}

		if (AIngine::Input::IsKeyPressed(AIngine::KeyCode::D))
		{
			camera->Translate(glm::vec2(translationrate, 0.0));
		}


		if (AIngine::Input::IsKeyPressed(AIngine::KeyCode::W))
		{
			camera->Translate(glm::vec2(0.0, -translationrate));
		}


		if (AIngine::Input::IsKeyPressed(AIngine::KeyCode::S))
		{
			camera->Translate(glm::vec2(0.0, +translationrate));
		}

		if (AIngine::Input::IsKeyPressed(AIngine::KeyCode::E))
		{
			camera->Rotate(rotationrate *  AIngine::D2R * delta);
		}

		if (AIngine::Input::IsKeyPressed(AIngine::KeyCode::Q))
		{
			camera->Rotate(-rotationrate * AIngine::D2R * delta);
		}
	}

	void Editor::MoveCameraWithMouse(float delta)
	{
		glm::vec2 currentMousePos = glm::vec2(Input::GetMouseX(), Input::GetMouseY());
		glm::vec2 del = currentMousePos - lastMousePos;
		m_app.m_camera->Translate(del * delta * m_app.m_camera->GetZoom());
		lastMousePos = currentMousePos;
	}

	// the path to the file that contains the path to the scene that was last opened
	static const std::string lastSceneFilePath = "Editor/lastScene.json";

	void Editor::OnWindowClose()
	{
		// remember the path to the scene
		std::ofstream file(lastSceneFilePath);
		file << m_currentSceneFilePath;
		file.close();

		if (m_isInPlayMode) {
			m_isInPlayMode = false;
			OnLeavePlayModeEvent();
		}
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

	std::string Editor::GetCurrentSceneName()
	{
		if (s_instance) {
			std::string Path(std::filesystem::canonical(s_instance->m_currentSceneFilePath).string());
			size_t first = Path.find_last_of('\\') + 1;
			size_t last = Path.find_last_of('.');
			return Path.substr(first, last - first).c_str();
		}
		return "";
	}

	void Editor::UpdateSceneTitle()
	{
		std::string title = AIngine::Util::Project::GetProjectName() + " | " + GetCurrentSceneName();
		m_app.m_window->SetWindowTitle(title.c_str());
	}


	Editor::Editor()
		: m_app(AIngine::Application::Get()), Layer("Editor")
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
		m_widgets.push_back(new Widget::SceneGraphWidget(*m_app.m_world->m_sceneGraph));
		m_widgets.push_back(new Widget::LogWidget());
		m_widgets.push_back(new Widget::CameraWidget(*m_app.m_camera));
		m_widgets.push_back(new Widget::MenubarWidget());
		m_widgets.push_back(new Widget::ToolbarWidget());

		AIngine::Editor::Serialization::Serializer::LoadEditorSettings();
		m_BuildScenes = AIngine::Editor::Serialization::Serializer::LoadBuildScenes();
	}

	void Editor::EnterFullScreenMode()
	{
		m_isFullScreen = true;
		m_app.m_window->SetFullScreen(true);
		AIngine::Structures::Rectangle viewportRect = AIngine::Structures::RectangleI(m_app.GetWindow().GetX(), 0, m_app.GetWindow().GetWidth(), m_app.GetWindow().GetHeight());
		OnViewportChangedEvent(viewportRect);
	}

	void Editor::LeaveFullScreenMode()
	{
		m_isFullScreen = false;
		m_app.m_window->SetFullScreen(false);
		AIngine::Structures::Rectangle viewportRect = CalculateViewportRect(glm::vec2(m_app.GetWindow().GetWidth(), m_app.GetWindow().GetHeight()));
		OnViewportChangedEvent(viewportRect);
	}

	AIngine::Structures::RectangleI Editor::CalculateViewportRect(const glm::vec2& windowSize) const
	{
		using Rectangle = AIngine::Structures::RectangleI;
		using Corner = AIngine::Structures::RectangleI::Corner;

		Rectangle viewportRect(0, m_widgets[3]->GetSize().y, windowSize.x, windowSize.y - m_widgets[3]->GetSize().y);

		auto it = m_widgets.begin();

		while (it != m_widgets.end()) {
			const Widget::EditorWidget& widget = *(*it._Ptr);
			if (widget.IsWindowDocked()) {
				AIngine::Structures::Rectangle widgetRect = widget.GetRectangle();
				if (viewportRect.Contains(widgetRect)) {
					glm::vec<2, glm::i32> windowPos = glm::vec<2, glm::i32>(m_app.m_window->GetX(), m_app.m_window->GetY());
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

	void Editor::SetShowFpsGraph(bool show)
	{
		if (s_instance)
			s_instance->m_showingFpsGraph = show;
	}

	void Editor::SetSelectedObjects(const std::vector<AIngine::GameObject*>& selectedObjects)
	{
		if (s_instance) s_instance->m_selectedObjects = selectedObjects;
		OnSelectionChangedEvent(selectedObjects);
	}

	void Editor::ResetSceneGraph()
	{
		m_app.m_world->GetSceneGraph().Reset();
	}

	void Editor::ResetSceneGraph(AIngine::Application * app)
	{
		app->m_world->m_sceneGraph->Reset();
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

	void Editor::SetFullScreenPlayMode(bool bFullsceen)
	{
		if (s_instance) {
			// leave fullscreen mode
			if (s_instance->m_isFullScreen && !bFullsceen) {
				s_instance->LeaveFullScreenMode();
			}

			// enter fullscreen mode
			if (!s_instance->m_isFullScreen && bFullsceen) {
				s_instance->EnterFullScreenMode();
			}
		}
	}

	std::vector<Scene> Editor::LoadBuildScenes()
	{
		return AIngine::Editor::Serialization::Serializer::LoadBuildScenes();
	}

	bool Editor::ContainsScene(const std::string & name)
	{
		if (s_instance) {
			for (auto& it = s_instance->m_BuildScenes.begin(); it != s_instance->m_BuildScenes.end(); it++) {
				if (it->Name == name)
				{
					return true;
				}
			}
		}
		return false;
	}

	void Editor::SaveBuildScenes()
	{
		if (s_instance) {
			AIngine::Editor::Serialization::Serializer::SaveBuildScenes(s_instance->m_BuildScenes);
		}
	}

	void Editor::AddCurrentSceneToBuild()
	{
		if (s_instance)
		{
			s_instance->m_BuildScenes.push_back(Scene
				{
					s_instance->GetCurrentSceneName(),
					std::filesystem::canonical(s_instance->m_currentSceneFilePath).string()
				});
		}
	}

	bool Editor::CreateMoveablePositionVertex(glm::vec2 & worldPosition, float vertexSize, const glm::vec4 & colorInteract, const glm::vec4 & colorNormal)
	{
		glm::vec2 mouseScreenPos = glm::vec2(AIngine::Input::GetMousePosition().first, AIngine::Input::GetMousePosition().second);
		glm::vec2 mouseWorldPos = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(mouseScreenPos);
		glm::vec2 screenPos = AIngine::Rendering::Camera::Get().WorldToScreenPoint(worldPosition);
		AIngine::Structures::RectangleI vertexRectangle(screenPos.x - vertexSize, screenPos.y - vertexSize, 2 * vertexSize, 2 * vertexSize);
		bool bInteracted = false;

		if (vertexRectangle.Contains(mouseScreenPos)) {
			if (AIngine::Input::IsMouseButtonPressed(MouseButton::BUTTON_LEFT)) {
				glm::vec2 diff = mouseWorldPos - worldPosition;
				worldPosition += diff;
				screenPos = AIngine::Rendering::Camera::Get().WorldToScreenPoint(worldPosition);
				vertexRectangle = AIngine::Structures::RectangleI(screenPos.x - vertexSize, screenPos.y - vertexSize, 2 * vertexSize, 2 * vertexSize);
				bInteracted = true;
			}
			AIngine::Graphics::Point(worldPosition, vertexSize, colorInteract);
		}
		else {
			AIngine::Graphics::Point(worldPosition, vertexSize, colorNormal);
		}

		AIngine::Graphics::BoxScreen(vertexRectangle, glm::vec4(1, 1, 0, 1));
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
			s_instance->m_currentSceneFilePath = std::filesystem::canonical(path).string();

			std::string sceneName = s_instance->GetCurrentSceneName();
			s_instance->UpdateSceneTitle();

			CORE_INFO("Loaded scene " + sceneName + " from " + path);
		}
		else {
			AIngine::Editor::Serialization::Serializer::DeserializeSceneGraph(path);
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
		static const char* filterList = "txt,json";
		std::string path;
		AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::OpenFile(filterList, &path, "scenes");

		if (result == AIngine::Util::Filesystem::Result::OKAY)
		{
			LoadScene(path);
		}
	}

	bool Editor::SaveSceneToFile()
	{
		static const char* filterList = "txt,json";
		std::string path;
		AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::SaveFile(filterList, &path, "scenes");

		if (result == AIngine::Util::Filesystem::Result::OKAY)
		{
			SaveScene(path);
			return true;
		}
		return false;
	}

	void Editor::DisplayFramerate(float delta) const
	{
		std::stringstream ss;
		ss << "FPS: " << (int)(1.0f / delta);
		static const glm::vec2 screenPos(5, 25);
		AIngine::Graphics::Text(ss.str().c_str(), screenPos, glm::vec2(0.5));
	}

	void Editor::DrawFpsGraph(float delta) const
	{
		static const ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
		static bool open = true;
		if (ImGui::Begin("fps", &open, flags)) {
			static bool firstFrame = true;
			if (firstFrame) {
				firstFrame = false;
				ImGui::End();
				return;
			}
			static std::vector<float> fpsqueue;
			static float max = 0;
			static float min = 1000;
			struct Funcs
			{
				static float Get(void*, int i) { return fpsqueue[i]; }

			};

			if (fpsqueue.size() >= 500) {
				if (*fpsqueue.begin()._Ptr == min) {
					min = 1000;
				}
				fpsqueue.erase(fpsqueue.begin());
			}
			float fps = 1.0f / delta;
			if (fps > max)
				max = fps;
			if (fps < min) {
				min = fps;
			}
			fpsqueue.push_back(fps);
			float(*func)(void*, int) = Funcs::Get;

			ImGui::PlotLines("FPS", func, NULL, fpsqueue.size(), 0, std::to_string(1.0 / delta).c_str(), min, max, ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()));

		}
		ImGui::End();
	}

	bool Editor::DidAnyDockedWidgetChangeSize() const
	{
		auto it = m_widgets.begin();

		while (it != m_widgets.end()) {
			const Widget::EditorWidget& widget = *(*it._Ptr);
			if (widget.WasWindowSizeChanged() && widget.IsWindowDocked())
				return true;
			it++;
		}
		return false;
	}

	Editor::~Editor()
	{

		for (unsigned int i = 0; i < m_widgets.size(); i++) {
			delete m_widgets[i];
		}
		m_widgets.clear();

		AIngine::Editor::Serialization::Serializer::SaveEditorSettings();

		s_instance = nullptr;
	}

	void Editor::Build()
	{
		if (s_instance) {
			if (s_instance->m_BuildScenes.size() == 0) return;

			//std::vector<std::string> Result;

			//for (auto& scene : s_instance->m_BuildScenes) {

			//	auto result = AIngine::Editor::Serialization::ExtractPathsFromScene(scene.Path);
			//	for (auto& path : result) {
			//		if (std::find(Result.begin(), Result.end(), path) == Result.end())
			//			Result.push_back(path);
			//	}
			//}

			std::string projectDir = AIngine::Util::Project::GetProjectDir();
			std::string path;
			AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::PickFolder(&path, nullptr);
			std::string outputDir;
			if (result == AIngine::Util::Filesystem::Result::OKAY)
			{
				static const std::string config = "Debug";
				outputDir = std::filesystem::canonical(path).string();
				std::filesystem::create_directories(outputDir + "\\" + config + "\\Editor");
				std::filesystem::create_directories(outputDir + "\\Resources");

				// Turn off Editor
				AIngine::Util::Project::RegenerateCMake({ "-DEDITOR=OFF", "-DOutputDir=" + outputDir });

				// Build
				std::stringstream command;
				command << projectDir[0] << ":" << " && ";
				command << "cd " << projectDir + "out//CMake" << " && ";
				command << "cmake --build . --target " << AIngine::Util::Project::GetProjectName() << " --config " << config;
				AIngine::Util::System::Exec({ command.str() }, false);

				// Turn on Editor again
				AIngine::Util::Project::RegenerateCMake({ "-DEDITOR=ON" });

				// Copy the file with the built scenes
				static const std::filesystem::copy_options copyoptions = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive;
				std::filesystem::copy_file("Editor\\ScenesBuild.json", outputDir + "\\" + config + "\\Editor\\ScenesBuild.json", copyoptions);
				// Copy Resources
				std::filesystem::copy(AIngine::Util::Project::GetResourceDirectory(), outputDir + "\\Resources", copyoptions);
				AIngine::Util::Project::SetProjectDir(outputDir, outputDir + "\\" + config + "\\project.proj");
			}
		}
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

	void Editor::SetPaused(bool pause)
	{
		if (s_instance)
		{
			if (s_instance->m_isGamePaused)
				if (!pause)
					Editor::ResumeGameEvent();
			if (!s_instance->m_isGamePaused)
				if (pause)
					Editor::PauseGameEvent();

			s_instance->m_isGamePaused = pause;
		}
	}

}

// initialize ViewportChangedEventHandler counter
int AIngine::Events::EventHandler<void, AIngine::Structures::RectangleI&>::counter = 0;
int AIngine::Editor::Editor::SelectionChangedEventHandler::counter = 0;