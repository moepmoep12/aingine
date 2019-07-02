#pragma once

#include <imgui.h>
#include <typeinfo>
#include <string>

namespace AIngine::Rendering {
	class Texture2D;
}

namespace AIngine::Editor::Widgets {

	template<class T>
	void DisplayTitle(T* obj, const char* title) {
		float textWidth = ImGui::CalcTextSize(title).x;
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) * 0.5f);
		ImGui::TextColored(ImVec4(1, 1, 0, 1), title);
		// ContextMenu
		if (ImGui::BeginPopupContextItem((std::string(title).append("##").append(typeid(T).name())).c_str())) {
			std::string action = obj->IsEnabled() ? "Deactivate" : "Activate";
			if (ImGui::Selectable((action + "##" + typeid(T).name()).c_str())) {
				obj->SetEnabled(!obj->IsEnabled());
			}
			if (ImGui::Selectable((std::string("Delete").append("##").append(typeid(T).name())).c_str())) {
				obj->GetOwner()->RemoveComponent(obj);
				ImGui::EndPopup();
				return;
			}
			ImGui::EndPopup();
		}
	}


	void PreviewImage(const AIngine::Rendering::Texture2D& texture);

	void TextureParams(AIngine::Rendering::Texture2D& texture);
}