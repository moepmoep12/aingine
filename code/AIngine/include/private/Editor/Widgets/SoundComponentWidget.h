#pragma once
#include "Editor/Widgets/ComponentWidget.h"
#include "Rendering/texture.h"

#include <map>
#include <string>

namespace AIngine::Editor {
	class SoundComponentWidget : public ComponentWidget {
	public:
		SoundComponentWidget();
		virtual ~SoundComponentWidget();
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;

	private:
		std::map<std::string, AIngine::Rendering::Texture2D> m_Icons;
	};
}