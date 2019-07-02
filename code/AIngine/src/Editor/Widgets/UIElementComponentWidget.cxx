#include "Editor/Widgets/UIElementComponentWidget.h"
#include "UI/UIELement.h"
#include "AIngine/Graphics.h"
#include "Rendering/Viewport.h"
#include "Application.h"

#include "imgui.h"

namespace AIngine::Editor::Widget::Component {

	void AnchorCombo(AIngine::UI::UIElement * element)
	{
		static const AIngine::UI::Anchor choosableAnchors[] = { AIngine::UI::Anchor::Center,AIngine::UI::Anchor::TopLeft, AIngine::UI::Anchor::TopRight, AIngine::UI::Anchor::BottomRight,
			AIngine::UI::Anchor::BottomLeft, AIngine::UI::Anchor::CenterUp,AIngine::UI::Anchor::CenterLeft, AIngine::UI::Anchor::CenterDown, AIngine::UI::Anchor::CenterRight };
		static const char* anchorNames[] = { "Center", "TopLeft", "TopRight", "BottomRight", "BottomLeft", "CenterUp", "CenterLeft", "CenterDown", "CenterRight" };

		/*AIngine::UI::Anchor currentAnchor = element->AnchorPos;*/
		if (ImGui::BeginCombo("Anchor", anchorNames[element->GetAnchor()])) {
			for (int i = 0; i < IM_ARRAYSIZE(anchorNames); i++) {
				bool isSelected = choosableAnchors[i] == element->GetAnchor();
				if (ImGui::Selectable(anchorNames[i], isSelected))
					element->SetAnchor( choosableAnchors[i]);
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		glm::vec2 anchorpos;
		const AIngine::Structures::RectangleI& viewportRect = AIngine::Application::GetViewport().GetRectangle();


		switch (element->GetAnchor()) {
		case AIngine::UI::Anchor::TopLeft:
			anchorpos = glm::vec2(0);
			break;

		case AIngine::UI::Anchor::TopRight:
			anchorpos = viewportRect.GetTopRight();
			break;

		case AIngine::UI::Anchor::BottomRight:
			anchorpos = viewportRect.GetMax();
			break;

		case AIngine::UI::Anchor::BottomLeft:
			anchorpos = viewportRect.GetBottomLeft();
			break;

		case AIngine::UI::Anchor::Center:
			anchorpos = viewportRect.GetCenter();
			break;

		case AIngine::UI::Anchor::CenterDown:
			glm::highp_ivec2 centerDown = viewportRect.GetMax();
			centerDown.x -= viewportRect.width * 0.5f;
			anchorpos = centerDown;
			break;

		case AIngine::UI::Anchor::CenterLeft:
			glm::highp_ivec2 centerLeft = viewportRect.GetPosition();
			centerLeft.y += viewportRect.height * 0.5f;
			anchorpos = centerLeft;
			break;

		case AIngine::UI::Anchor::CenterRight:
			glm::highp_ivec2 centerRight = viewportRect.GetTopRight();
			centerRight.y += viewportRect.height * 0.5f;
			anchorpos = centerRight;
			break;

		case AIngine::UI::Anchor::CenterUp:
			glm::highp_ivec2 centerUp = viewportRect.GetTopRight();
			centerUp.x -= viewportRect.width * 0.5f;
			anchorpos = centerUp;
			break;
		}

		AIngine::Graphics::PointScreen(anchorpos, 50 , glm::vec3(0.35, 0.72, 0));
	}

	void ChangeTransform(AIngine::UI::UIElement * element)
	{
		int pos[] = { element->GetRectangleNative().x, element->GetRectangleNative().y };
		int dim[] = { element->GetRectangleNative().width, element->GetRectangleNative().height };
		if (ImGui::DragInt2("Screen Position", pos)) {
			element->SetPosition(glm::vec2(pos[0], pos[1]));
		}
		if (ImGui::DragInt2("Dimensions", dim)) {
			element->SetWidth(dim[0]);
			element->SetHeight(dim[1]);
		}

		// Disabled
		bool isDisabled = element->IsDisabled();
		if (ImGui::Checkbox("Disabled", &isDisabled)) {
			element->SetDisabled(isDisabled);
		}
	}
	void ColorEdit(float * Color, const char * title)
	{
		static const ImGuiColorEditFlags flags = ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar;
		ImGui::ColorEdit4(title, Color, flags);
	}
}