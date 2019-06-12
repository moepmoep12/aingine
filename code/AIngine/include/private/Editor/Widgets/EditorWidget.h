#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "Events/Event.h"
#include "Structures/Rectangle.h"

#include <glm/glm.hpp>

namespace AIngine::Editor {

	/*
	* The base class for widgets that make up the Editor
	*/
	class EditorWidget {
	public:

		/* Called once per frame when the widget will be drawn*/
		virtual void OnImGuiRender() = 0;

		/* can be used to react to input events*/
		virtual void OnEvent(AIngine::Events::EventData& e) {}

		virtual const glm::vec2& GetSize() const { return m_size; }
		virtual const glm::vec2& GetPosition() const { return m_position; }
		virtual bool IsWindowDocked() const { return m_isDocked; }
		virtual bool WasWindowSizeChanged() const { return m_wasSizeChanged; }

		virtual AIngine::Structures::Rectangle GetRectangle() const {
			return AIngine::Structures::Rectangle(m_position.x, m_position.y, m_size.x, m_size.y);
		}

		virtual ~EditorWidget() {}

	protected:
		glm::vec2 m_size = glm::vec2(0);
		glm::vec2 m_position = glm::vec2(0);
		bool m_isDocked = false;
		bool m_wasSizeChanged = true;
	};
}

namespace ImGui {
	// label is used as id
	// <0 frame_padding uses default frame padding settings. 0 for no padding
	inline IMGUI_API bool ImageButtonWithText(ImTextureID texId, const char* label, const ImVec2& imageSize = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1)) {
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImVec2 size = imageSize;
		if (size.x <= 0 && size.y <= 0) { size.x = size.y = ImGui::GetTextLineHeightWithSpacing(); }
		else {
			if (size.x <= 0)          size.x = size.y;
			else if (size.y <= 0)     size.y = size.x;
			size.x *= window->FontWindowScale*ImGui::GetIO().FontGlobalScale;
			size.y *= window->FontWindowScale*ImGui::GetIO().FontGlobalScale;
		}

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		ImGuiID id = window->GetID(label);
		const ImVec2 textSize = ImGui::CalcTextSize(label, NULL, true);
		const bool hasText = textSize.x > 0;

		const float innerSpacing = hasText ? ((frame_padding >= 0) ? (float)frame_padding : (style.ItemInnerSpacing.x)) : 0.f;
		ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
		ImVec2 totalSizeWithoutPadding(size.x + innerSpacing + textSize.x, size.y > textSize.y ? size.y : textSize.y);
		ImVec2 temp(0, 0);
		temp.x = window->DC.CursorPos.x + totalSizeWithoutPadding.x + padding.x * 2;
		temp.y = window->DC.CursorPos.y + totalSizeWithoutPadding.y + padding.y * 2;
		const ImRect bb(window->DC.CursorPos, temp);
		ImVec2 start(0, 0);
		start.x = window->DC.CursorPos.x + padding.x;
		start.y = window->DC.CursorPos.y + padding.y;
		if (size.y < textSize.y) start.y += (textSize.y - size.y)*.5f;
		temp.x = start.x + size.x;
		temp.y = start.y + size.y;
		const ImRect image_bb(start, temp);
		start.x = window->DC.CursorPos.x + padding.x; 
		start.y = window->DC.CursorPos.y + padding.y; 
		start.x += size.x + innerSpacing;
		if (size.y > textSize.y) start.y += (size.y - textSize.y)*.5f;
		ItemSize(bb);
		if (!ItemAdd(bb, id))
			return false;

		bool hovered = false, held = false;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held);

		// Render
		const ImU32 col = GetColorU32((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
		if (bg_col.w > 0.0f)
			window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));

		window->DrawList->AddImage(texId, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col));

		if (textSize.x > 0) ImGui::RenderText(start, label);
		return pressed;
	}
} // namespace ImGui