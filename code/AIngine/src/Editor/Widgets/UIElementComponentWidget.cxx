#include "Editor/Widgets/UIElementComponentWidget.h"
#include "UI/UIELement.h"
#include "AIngine/Graphics.h"
#include "Rendering/Viewport.h"
#include "Application.h"

#include "imgui.h"
#include <sstream>
#include <string>

namespace AIngine::Editor::Widget::Component {

	bool AnchorCombo(AIngine::UI::UIElement * element)
	{
		bool changed = false;

		static const AIngine::UI::Anchor choosableAnchors[] = { AIngine::UI::Anchor::Center,AIngine::UI::Anchor::TopLeft, AIngine::UI::Anchor::TopRight, AIngine::UI::Anchor::BottomRight,
			AIngine::UI::Anchor::BottomLeft, AIngine::UI::Anchor::CenterUp,AIngine::UI::Anchor::CenterLeft, AIngine::UI::Anchor::CenterDown, AIngine::UI::Anchor::CenterRight };
		static const char* anchorNames[] = { "Center", "TopLeft", "TopRight", "BottomRight", "BottomLeft", "CenterUp", "CenterLeft", "CenterDown", "CenterRight" };

		const void * address = static_cast<const void*>(element);
		std::stringstream ss;
		ss << address;
		std::string name = ss.str();

		std::string anchorname = "Anchor##" + name;
		if (ImGui::BeginCombo(anchorname.c_str(), anchorNames[(int)element->GetAnchor()])) {
			for (int i = 0; i < IM_ARRAYSIZE(anchorNames); i++) {
				bool isSelected = choosableAnchors[i] == element->GetAnchor();
				if (ImGui::Selectable(anchorNames[i], isSelected)) {
					element->SetAnchor(choosableAnchors[i]);
					changed = true;
				}
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

		AIngine::Graphics::PointScreen(anchorpos, 50, glm::vec3(0.35, 0.72, 0));

		return changed;
	}

	bool ChangeTransform(AIngine::UI::UIElement * element)
	{
		bool changed = false;
		const void * address = static_cast<const void*>(element);
		std::stringstream ss;
		ss << address;
		std::string name = ss.str();

		std::string screenposname = "Screen Position##" + name;
		std::string dimname = "Dimensions##" + name;

		int pos[] = { element->GetRectangleNative().x, element->GetRectangleNative().y };
		int dim[] = { element->GetRectangleNative().width, element->GetRectangleNative().height };
		if (ImGui::DragInt2(screenposname.c_str(), pos)) {
			element->SetPosition(glm::vec2(pos[0], pos[1]));
			changed = true;
		}
		if (ImGui::DragInt2(dimname.c_str(), dim)) {
			element->SetWidth(dim[0]);
			element->SetHeight(dim[1]);
			changed = true;
		}

		// Disabled
		std::string disabledname = "Disabled##" + name;
		bool isDisabled = element->IsDisabled();
		if (ImGui::Checkbox(disabledname.c_str(), &isDisabled)) {
			element->SetDisabled(isDisabled);
			changed = true;
		}

		return changed;
	}
	bool ColorEdit(float * Color, const char * title)
	{
		bool changed = false;

		static const ImGuiColorEditFlags flags = ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar;
		if (ImGui::ColorEdit4(title, Color, flags)) {
			changed = true;
		}
		return changed;
	}
}