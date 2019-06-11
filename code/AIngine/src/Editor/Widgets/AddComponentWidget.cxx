#include "Editor/Widgets/AddComponentWidget.h"
#include "AIngine/GameObject.h"
#include "AIngine/Sprite.h"
#include "AIngine/Physics.h"

namespace AIngine::Editor {

	void AddComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1)
		{
			AIngine::GameObject* obj = m_activeGameObjects[0];
			float windowWidth = ImGui::GetWindowWidth();
			float buttonWidth = windowWidth - windowWidth * 0.15;
			static bool active = false;


			ImGui::Separator();
			//ImGui::Indent((windowWidth - buttonWidth) * 0.5);
			if (ImGui::Button("AddComponent", ImVec2(buttonWidth,0))) {
				active = !active;
			}

			if (active) {
				static ImGuiTextFilter filter;
				static const char* componentNames[] = { "Sprite", "Physics" };
				filter.Draw("", buttonWidth);

				for (int i = 0; i < IM_ARRAYSIZE(componentNames); i++) {
					if (filter.PassFilter(componentNames[i])) {
						if (ImGui::Selectable(componentNames[i])) {
							switch (i) {
							case 0:
								if (!obj->GetComponent<AIngine::Sprite>()) {
									obj->AddComponent<AIngine::Sprite>();
									break;
								}

							case 1:
								if (!obj->GetComponent<AIngine::Physics::PhysicsComponent>()) {
									obj->AddComponent<AIngine::Physics::PhysicsComponent>();
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}