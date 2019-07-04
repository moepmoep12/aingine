#include "Editor/Widgets/AddComponentWidget.h"
#include "AIngine/GameObject.h"
#include "AIngine/Sprite.h"
#include "AIngine/Physics.h"
#include "AIngine/SoundComponent.h"
#include "AIngine/ParticleEmitter.h"
#include "UI/UIELement.h"
#include "UI/Button.h"
#include "UI/Image.h"
#include "UI/Text.h"
#include "UI/CheckBox.h"
#include "UI/Slider.h"
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
				static const int engineComponentsCount = 10;
				ImGui::BeginChild("addComponentChildWindow", ImVec2(buttonWidth, 30 * engineComponentsCount), true, 0);

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
							case 6:
								if (!obj->GetComponent<AIngine::UI::Image>()) {
									obj->AddComponent<AIngine::UI::Image>();
									break;
								}
							case 7:
								if (!obj->GetComponent<AIngine::UI::UIText>()) {
									obj->AddComponent<AIngine::UI::UIText>();
									break;
								}
							case 8:
								if (!obj->GetComponent<AIngine::UI::CheckBox>()) {
									obj->AddComponent<AIngine::UI::CheckBox>();
									break;
								}
							case 9:
								if (!obj->GetComponent<AIngine::UI::Slider>()) {
									obj->AddComponent<AIngine::UI::Slider>();
									break;
								}
							default:
								AIngine::OnAddComponent(obj, i - engineComponentsCount);
								break;
							}
						}
						if (i == engineComponentsCount -1)
							ImGui::Separator();

					}
				}
				ImGui::EndChild();
			}
		}
	}
}