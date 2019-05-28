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

		static AIngine::Memory::Pool<GameObject>* GetGameObjectPool();
		static AIngine::Structures::SceneGraph* GetSceneGraph();
		static bool IsAnyUIElementHovered();
		bool DidAnyDockedWidgetChangeSize() const;

		AIngine::Structures::Rectangle CalculateViewportRect(const glm::vec2& windowSize) const;

		virtual ~Editor() override;


	private:
		bool OnKeyPressed(AIngine::Events::KeyPressedEvent& e);
		bool OnWindowResized(AIngine::Events::WindowResizeEvent& e);
		Editor();

	private:
		static Editor* s_instance;
		AIngine::Application& m_app;
		std::vector<EditorWidget*> m_widgets;

	};
}