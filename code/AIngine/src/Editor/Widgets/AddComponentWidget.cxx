#include "Editor/Widgets/AddComponentWidget.h"
#include "AIngine/GameObject.h"
#include "AIngine/Sprite.h"
#include "AIngine/Physics.h"
#include "AIngine/SoundComponent.h"
#include "AIngine/ParticleEmitter.h"
#include "UI/UIELement.h"
#include "UI/Button.h"
#include "Rendering/UIRenderer.h"
#include "Application.h"

#include <vector>
#include <string>

namespace AIngine::Editor::Widget::Component {
	AddComponentWidget::AddComponentWidget()
	{

	}

	AddComponentWidget::~AddComponentWidget()
	{
	}

	void AddComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1)
		{
			AIngine::GameObject* obj = m_activeGameObjects[0];
			float windowWidth = ImGui::GetWindowWidth();
			float buttonWidth = windowWidth - (windowWidth * 0.15f);
			static bool active = false;
			static const float buttonHeight = 40;

			ImGui::Separator();
			ImGui::NewLine();
			ImGui::NewLine();

			ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
			if (ImGui::Button("Add Component##addComponentWidget", ImVec2(buttonWidth, buttonHeight))) {
				active = !active;
			}

			if (active) {
				static ImGuiTextFilter filter;
				std::vector<std::string > componentNames = AIngine::GetAvailableComponentNames();

				ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

				ImGui::BeginChild("addComponentChildWindow", ImVec2(buttonWidth, 30 * 3), true, 0);

				filter.Draw("", buttonWidth);

				for (int i = 0; i < componentNames.size(); i++) {
					if (filter.PassFilter(componentNames[i].c_str())) {
						if (ImGui::Selectable(componentNames[i].c_str())) {
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
							case 2:
								if (!obj->GetComponent<AIngine::SoundComponent>()) {
									obj->AddComponent<AIngine::SoundComponent>();
									break;
								}
							case 3:
								if (!obj->GetComponent<AIngine::ParticleEmitter>()) {
									obj->AddComponent<AIngine::ParticleEmitter>();
									break;
								}
							case 4:
								if (!obj->GetComponent<AIngine::UI::Canvas>()) {
									if (AIngine::Rendering::UIRenderer::canvas == nullptr) {
										obj->AddComponent<AIngine::UI::Canvas>();
										AIngine::Rendering::UIRenderer::canvas = obj;
									}
									break;
								}
							case 5:
								if (!obj->GetComponent<AIngine::UI::Button>()) {
									obj->AddComponent<AIngine::UI::Button>();
									break;
								}
							default:
								AIngine::OnAddComponent(obj, i - 6);
								break;
							}
						}
					}
				}
				ImGui::EndChild();
			}
		}
	}
}