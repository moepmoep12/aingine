#pragma once

#include "Structures/Layer.h"
#include "Structures/Rectangle.h"
#include "Events/InputEvents.h"
#include "Events/ApplicationEvents.h"
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

		/*  Resets the scene graph by deleting all nodes and creating a new root node */
		static void ResetSceneGraph();
		static AIngine::Structures::SceneGraph* GetSceneGraph();

		/* Returns wheter any UI element is currently hovered with the mouse */
		static bool IsAnyUIElementHovered();

		/* Calculates the available viewport size to be used for rendering the scene
		* @returns : Returns the viewport Rectangle with a screen position & size */
		AIngine::Structures::Rectangle CalculateViewportRect(const glm::vec2& windowSize) const;

		/* Actives/Deactivates the frame counter in the top left corner*/
		static void SetShowFramerate(bool active);
		static inline bool IsFramerateDisplayed() { if (s_instance) return s_instance->m_displayingFramerate; else return false; }

		static inline void SetSelectedObjects(const std::vector<AIngine::GameObject*>& selectedObjects) { if (s_instance) s_instance->m_selectedObjects = selectedObjects; }
		static inline const std::vector<AIngine::GameObject*>* GetSelectedObjects() { if (s_instance) return &s_instance->m_selectedObjects; else return nullptr; }

		virtual ~Editor() override;

		static inline bool GetIsInPlayMode() { if (s_instance) return s_instance->m_isInPlayMode; else return true; }
		static void SetIsInPlayMode(bool value);

		static inline std::string GetCurrentSceneFilePath() { if (s_instance) return s_instance->m_currentScene; else return std::string(); }
		static void SetCurrentSceneFilePath(const std::string& path) { if (s_instance) s_instance->m_currentScene = path; }

		static bool CreateMoveablePositionVertex(glm::vec2& worldPosition, float vertexSize, const glm::vec3& colorInteract = glm::vec3(0, 1, 0), const glm::vec3& colorNormal = glm::vec3(1, 0, 0));
	private:
		void DisplayFramerate(float delta) const;

		/* Checks whether a docked Editor widget changed its size in the last frame to be able to adjust the viewport accordingly*/
		bool DidAnyDockedWidgetChangeSize() const;
		bool OnKeyPressed(AIngine::Events::KeyPressedEvent::KeyPressedEventData& e);
		bool OnWindowResized(AIngine::Events::WindowResizeEvent::WindowResizeEventData& e);
		bool OnMouseScrolled(AIngine::Events::MouseScrolledEvent::MouseScrolledEventData& e);
		void MoveCamera(float delta);
		void OnWindowClose();
		void LoadLastScene();
		void SaveOpenScene();
		Editor();

	private:
		bool m_displayingFramerate = true;
		static Editor* s_instance;
		AIngine::Application& m_app;
		std::vector<EditorWidget*> m_widgets;
		std::vector<AIngine::GameObject*> m_selectedObjects;
		AIngine::Events::ViewportChangedEvent OnViewportChangedEvent;
		bool m_isInPlayMode = false;
		std::string m_currentScene;
	};
}