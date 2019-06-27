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
#include <algorithm>
#include <stdlib.h>

namespace AIngine::Editor {

	Editor* Editor::Editor::s_instance = nullptr;
	AIngine::Events::Event<void> Editor::PauseGameEvent;
	AIngine::Events::Event<void> Editor::ResumeGameEvent;

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
		// don't draw widgets if in fullscreen mode
		if (!m_isFullScreen) {
			auto it = m_widgets.begin();

			while (it != m_widgets.end()) {
				(*it._Ptr)->OnImGuiRender();
				it++;
			}

			if (m_showingFpsGraph) DrawFpsGraph(m_app.GetDeltaTime());
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

		if (e.GetKeyCode() == AIngine::KeyCodes::F4)
		{
			SetFullScreenPlayMode(!m_isFullScreen);
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
	static const std::string lastSceneFilePath = "Editor/lastScene.json";

	void Editor::OnWindowClose()
	{
		// remember the path to the scene
		std::ofstream file(lastSceneFilePath);
		file << m_currentSceneFilePath;
		file.close();

		if (m_isInPlayMode) {
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
		std::string sceneName = " | ";
		sceneName.append(GetCurrentSceneName());
		m_app.m_window->AppendWindowTitle(sceneName.c_str());
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
		m_widgets.push_back(new SceneGraphWidget(*m_app.m_world->m_sceneGraph));
		m_widgets.push_back(new LogWidget());
		m_widgets.push_back(new CameraWidget(*m_app.m_camera));
		m_widgets.push_back(new MenubarWidget());
		m_widgets.push_back(new ToolbarWidget());

		AIngine::Editor::Serialization::Serializer::LoadEditorSettings();
		m_BuildScenes = AIngine::Editor::Serialization::Serializer::LoadBuildScenes();
		LoadProjectData();
	}

	void Editor::EnterFullScreenMode()
	{
		m_isFullScreen = true;
		AIngine::Structures::Rectangle viewportRect = AIngine::Structures::RectangleI(m_app.GetWindow().GetX(), 0, m_app.GetWindow().GetWidth(), m_app.GetWindow().GetHeight());
		OnViewportChangedEvent(viewportRect);
	}

	void Editor::LeaveFullScreenMode()
	{
		m_isFullScreen = false;
		AIngine::Structures::Rectangle viewportRect = CalculateViewportRect(glm::vec2(m_app.GetWindow().GetWidth(), m_app.GetWindow().GetHeight()));
		OnViewportChangedEvent(viewportRect);
	}

	const char* s_projectFilePath = "project.proj";

	void Editor::LoadProjectData()
	{
		// open the file
		std::ifstream file;
		file.open(s_projectFilePath);
		if (file.fail()) return;
		nlohmann::json j = nlohmann::json::parse(file);
		file.close();

		m_projectName = j.at("name");
		m_resourceFolderPath = j.at("path");
		m_resourceFolderPath.append("Resources\\");
		m_engineInstallDir = j.at("installPath");
	}

	AIngine::Structures::RectangleI Editor::CalculateViewportRect(const glm::vec2& windowSize) const
	{
		using Rectangle = AIngine::Structures::RectangleI;
		using Corner = AIngine::Structures::RectangleI::Corner;

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

	void Editor::SetShowFpsGraph(bool show)
	{
		if (s_instance)
			s_instance->m_showingFpsGraph = show;
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

	std::string Editor::GetEngineInstallDirectory()
	{
		if (s_instance)
		{
			return s_instance->m_engineInstallDir;
		}
		else {
			// open the file
			std::ifstream file;
			file.open(s_projectFilePath);
			if (file.fail()) return "";
			nlohmann::json j = nlohmann::json::parse(file);
			file.close();
			std::string path = j.at("installPath");
			return path;
		}

		return std::string();
	}

	std::string Editor::GetResourceDirectory()
	{
		if (s_instance)
		{
			return s_instance->m_resourceFolderPath;
		}
		else {
			// open the file
			std::ifstream file;
			file.open(s_projectFilePath);
			if (file.fail()) return "";
			nlohmann::json j = nlohmann::json::parse(file);
			file.close();
			std::string path = j.at("path");
			path.append("Resources\\");
			return path;
		}

		return std::string();
	}

	std::string AIngine::Editor::Editor::GetProjectDirectory()
	{
		// open the file
		std::ifstream file;
		file.open(s_projectFilePath);
		if (file.fail()) return "";
		nlohmann::json j = nlohmann::json::parse(file);
		file.close();
		std::string path = j.at("path");
		return path;
	}

	std::string Editor::GetProjectName()
	{
		if (s_instance)
		{
			return s_instance->m_projectName;
		}
		else {
			// open the file
			std::ifstream file;
			file.open(s_projectFilePath);
			if (file.fail()) return "";
			nlohmann::json j = nlohmann::json::parse(file);
			file.close();

			return j.at("name");
		}
		return std::string();
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

	bool Editor::CreateMoveablePositionVertex(glm::vec2 & worldPosition, float vertexSize, const glm::vec3 & colorInteract, const glm::vec3 & colorNormal)
	{
		glm::vec2 mouseScreenPos = glm::vec2(AIngine::Input::GetMousePosition().first, AIngine::Input::GetMousePosition().second);
		glm::vec2 mouseWorldPos = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(mouseScreenPos);
		glm::vec2 screenPos = AIngine::Rendering::Camera::Get().WorldToScreenPoint(worldPosition);
		AIngine::Structures::RectangleI vertexRectangle(screenPos.x - vertexSize, screenPos.y - vertexSize, 2 * vertexSize, 2 * vertexSize);
		bool bInteracted = false;

		if (vertexRectangle.Contains(mouseScreenPos)) {
			if (AIngine::Input::IsMouseButtonPressed(0)) {
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
		AIngine::Graphics::Text(ss.str().c_str(), glm::vec2(0, 5), glm::vec2(0.5));
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
			const EditorWidget& widget = *(*it._Ptr);
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

			std::vector<std::string> Result;

			for (auto& scene : s_instance->m_BuildScenes) {

				auto result = AIngine::Editor::Serialization::ExtractPathsFromScene(scene.Path);
				for (auto& path : result) {
					if (std::find(Result.begin(), Result.end(), path) == Result.end())
						Result.push_back(path);
				}
			}

			nfdchar_t *outPath = NULL;
			nfdresult_t result = NFD_PickFolder("", &outPath);

			if (result == NFD_OKAY)
			{
				std::stringstream command;

				//// move to correct hard drive
				//command << outPath[0] << ":" << " && ";

				//// move to folder
				//command << "cd " << outPath << " && ";

				//// build
				//command << "cmake --build . --target " << s_instance->m_app.GetProjectName() << " --config Release";
				//system(command.str().c_str());

				free(outPath);
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