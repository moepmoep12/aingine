#pragma once
#include "Editor/Widgets/ComponentWidget.h"

namespace AIngine::Editor {
	class ParticleEmitterWidget : public ComponentWidget {
	public:
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;
	};
}