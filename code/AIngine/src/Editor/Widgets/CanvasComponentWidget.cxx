#include "Editor/Widgets/CanvasComponentWidget.h"
#include "UI/UIELement.h"
#include "AIngine/Graphics.h"

namespace AIngine::Editor::Widget::Component {
	void CanvasComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			AIngine::GameObject* obj = m_activeGameObjects[0];
			using Canvas = AIngine::UI::Canvas;
			Canvas* canvas = obj->GetComponent<Canvas>();
			if (canvas)
			{
				// Title
				float textWidth = ImGui::CalcTextSize("Canvas").x;
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) * 0.5f);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Canvas");
				// ContextMenu
				if (ImGui::BeginPopupContextItem("Canvas")) {
					std::string action = canvas->IsActive() ? "Deactivate" : "Activate";
					action.append("##canvas");
					if (ImGui::Selectable(action.c_str())) {
						canvas->SetEnabled(!canvas->IsActive());
					}
					if (ImGui::Selectable("Delete##canvas")) {
						obj->RemoveComponent<Canvas>();
						ImGui::EndPopup();
						return;
					}
					ImGui::EndPopup();
				}

				ImGui::NewLine();

				// Change rect transform
				AIngine::Structures::Rectangle<int> rect = canvas->GetRectangle();
				int* pos[] = { &rect.x, &rect.y };
				int* dim[] = { &rect.width, &rect.height };
				ImGui::DragInt2("Screen Position", *pos);
				ImGui::DragInt2("Dimensions", *dim);
				canvas->SetRectangle(rect);

				// Draw button rect
				AIngine::Graphics::BoxScreen(rect, glm::vec3(1, 1, 0));

				ImGui::NewLine();
				ImGui::Separator();
			}
		}
	}
}