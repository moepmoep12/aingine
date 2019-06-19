#pragma once

#include "Structures/Layer.h"
#include "Structures/Rectangle.h"
#include "Events/InputEvents.h"
#include "Events/ApplicationEvents.h"
#include "Events/Event.h"
#include "Structures/memory.h"

#include <glm/glm.hpp>
#include <vector>

namespace AIngine {
	class Application;
	class GameObject;

	namespace Structures {
		class SceneGraph;
	}
}

namespace AIngine::Editor {

	struct Scene {
		std::string Name, Path;
	};

	// forward declaration
	class EditorWidget;

	class Editor : public AIngine::Structures::Layer {

		// Application should create the Editor
		friend class Application;

	public:
		// Inherited via Layer
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float delta) override;
		virtual void OnEvent(AIngine::Events::EventData& e) override;
		virtual void OnImGuiRender() override;

		template<class T>
		inline static T* GetWidget() {
			if (s_instance) {
				auto it = Editor::s_instance->m_widgets.begin();

				while (it != Editor::s_instance->m_widgets.end())
				{
					if (typeid(T) == typeid(**it._Ptr)) {
						return dynamic_cast<T*>(*it._Ptr);
					}
					it++;
				}
			}
			return nullptr;
		}

		static AIngine::Structures::SceneGraph* GetSceneGraph();

		/* Returns wheter any UI element is currently hovered with the mouse */
		static bool IsAnyUIElementHovered();

		/* Calculates the available viewport size to be used for rendering the scene
		* @returns : Returns the viewport Rectangle with a screen position & size */
		AIngine::Structures::RectangleI CalculateViewportRect(const glm::vec2& windowSize) const;

		/* Actives/Deactivates the frame counter in the top left corner*/
		static void SetShowFramerate(bool active);
		static inline bool IsFramerateDisplayed() { if (s_instance) return s_instance->m_displayingFramerate; else return false; }

		/* Activates/Deactivates the fps graph*/
		static void SetShowFpsGraph(bool show);
		static inline bool IsFpsGraphVisible() { if (s_instance) return s_instance->m_showingFpsGraph; else return false; }

		static inline void SetSelectedObjects(const std::vector<AIngine::GameObject*>& selectedObjects) { if (s_instance) s_instance->m_selectedObjects = selectedObjects; }
		static inline const std::vector<AIngine::GameObject*>* GetSelectedObjects() { if (s_instance) return &s_instance->m_selectedObjects; else return nullptr; }

		virtual ~Editor() override;

		static inline bool IsGameRunning() { if (s_instance) return s_instance->m_isInPlayMode && !s_instance->m_isGamePaused; else return true; }

		static inline bool GetIsInPlayMode() { if (s_instance) return s_instance->m_isInPlayMode; else return true; }
		static void SetIsInPlayMode(bool value);

		static inline bool IsPaused() { if (s_instance) return s_instance->m_isGamePaused; else return false; }
		static inline void SetPaused(bool pause) { if (s_instance) s_instance->m_isGamePaused = pause; }

		static inline bool IsFullScreenPlayMode() { if (s_instance) return s_instance->m_isFullScreen; else return true; }
		static void SetFullScreenPlayMode(bool bFullsceen);

		static inline std::string GetCurrentSceneFilePath() { if (s_instance) return s_instance->m_currentSceneFilePath; else return std::string(); }
		//static void SetCurrentSceneFilePath(const std::string& path) { if (s_instance) s_instance->m_currentSceneFilePath = path; }

		static  std::vector<Scene>* GetBuildScenes() { if (s_instance) return &s_instance->m_BuildScenes; else return nullptr; }
		static std::vector<Scene> LoadBuildScenes();
		static bool ContainsScene(const std::string &name);
		static void SaveBuildScenes();
		static void AddCurrentSceneToBuild();

		static bool CreateMoveablePositionVertex(glm::vec2& worldPosition, float vertexSize, const glm::vec3& colorInteract = glm::vec3(0, 1, 0), const glm::vec3& colorNormal = glm::vec3(1, 0, 0));

		static void CreateNewScene();
		static void LoadScene(const std::string& path);
		static void SaveScene(const std::string& path);
		static void LoadSceneFromFile();
		static bool SaveSceneToFile();
		static std::string GetCurrentSceneName();

		AIngine::Events::Event<void> OnEnterPlayModeEvent;
		AIngine::Events::Event<void> OnLeavePlayModeEvent;


	private:
		void ResetSceneGraph();
		static void ResetSceneGraph(AIngine::Application* app);
		void DisplayFramerate(float delta) const;
		void DrawFpsGraph(float delta) const;

		/* Checks whether a docked Editor widget changed its size in the last frame to be able to adjust the viewport accordingly*/
		bool DidAnyDockedWidgetChangeSize() const;
		/* Callbacks*/
		bool OnKeyPressed(AIngine::Events::KeyPressedEvent::KeyPressedEventData& e);
		bool OnWindowResized(AIngine::Events::WindowResizeEvent::WindowResizeEventData& e);
		bool OnMouseScrolled(AIngine::Events::MouseScrolledEvent::MouseScrolledEventData& e);
		void OnWindowClose();

		void MoveCamera(float delta);

		void LoadLastScene();
		void SaveOpenScene();

		void UpdateSceneTitle();

		void EnterFullScreenMode();
		void LeaveFullScreenMode();

		Editor();

	private:
		bool m_displayingFramerate = false;
		bool m_showingFpsGraph = false;
		bool m_isFullScreen = false;
		static Editor* s_instance;
		AIngine::Application& m_app;
		std::vector<EditorWidget*> m_widgets;
		std::vector<AIngine::GameObject*> m_selectedObjects;
		std::vector<Scene> m_BuildScenes;
		AIngine::Events::ViewportChangedEvent OnViewportChangedEvent;
		bool m_isInPlayMode = false;
		bool m_isGamePaused = false;
		std::string m_currentSceneFilePath;
	};
}