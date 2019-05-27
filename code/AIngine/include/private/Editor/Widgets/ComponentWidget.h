#pragma once

#include "Editor/Widgets/EditorWidget.h"

#include <vector>

namespace AIngine {
	class GameObject;
}

namespace AIngine::Editor {
	class ComponentWidget : protected EditorWidget {
	public:
		virtual void OnImGuiRender() = 0;

		inline void Render(const std::vector<GameObject*>& objects) {
			m_activeGameObjects.clear();
			m_activeGameObjects = objects;
			OnImGuiRender();
		}

		virtual ~ComponentWidget() {
			m_activeGameObjects.clear();
		}

	protected:
		std::vector<GameObject*> m_activeGameObjects;
	};
}