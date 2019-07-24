#pragma once
#include "AIngine/Core.h"
#include "AIngine/UI/Text.h"
#include "AIngine/UI/Image.h"

namespace CrappyBird {
	class EffectDurationDisplay : public AIngine::Script {
	public:
		EffectDurationDisplay();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

		void SetYPos(int ypos);
		void Activate(float remaining);

		AIngine::UI::UIText* m_durationText = nullptr;
		AIngine::UI::Image* m_backGroundImage = nullptr;
		AIngine::UI::Image* m_effectImage = nullptr;

		float Remaining = 0;

	};
}