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
		virtual void OnEvent(AIngine::Events::Event& e) override;
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

		static void SetDisplayFramerateActive(bool active);

		virtual ~Editor() override;


	private:
		void DisplayFramerate(float delta) const;

		/* Checks whether a docked Editor widget changed its size in the last frame to be able to adjust the viewport accordingly*/
		bool DidAnyDockedWidgetChangeSize() const;
		bool OnKeyPressed(AIngine::Events::KeyPressedEvent& e);
		bool OnWindowResized(AIngine::Events::WindowResizeEvent& e);
		bool OnMouseScrolled(AIngine::Events::MouseScrolledEvent& e);
		void MoveCamera(float delta);
		Editor();

	private:
		bool m_displayingFramerate = true;
		static Editor* s_instance;
		AIngine::Application& m_app;
		std::vector<EditorWidget*> m_widgets;
	};
}