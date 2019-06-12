#pragma once

#include "Editor/Widgets/ComponentWidget.h"
#include "Rendering/texture.h"

#include <string>
#include <map>

namespace AIngine::Editor {

	class AddComponentWidget : public ComponentWidget {
	public:
		AddComponentWidget();
		virtual ~AddComponentWidget();
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;
	private:
		std::map<std::string, AIngine::Rendering::Texture2D> m_Icons;
	};
}