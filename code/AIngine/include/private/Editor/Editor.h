#pragma once

#include "Structures/Layer.h"
#include <glm/glm.hpp>
#include <vector>

namespace AIngine {
	class Application;
}

namespace AIngine::Editor {

	class ImGuiWidget;

	class Editor : public AIngine::Layer {

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

		static bool IsAnyUIElementHovered();

		glm::vec4 GetEditorBounds() const;

		virtual ~Editor() override;



	private:
		Editor();

	private:
		static Editor* s_instance;
		AIngine::Application& m_app;
		std::vector<ImGuiWidget*> m_widgets;

	};
}