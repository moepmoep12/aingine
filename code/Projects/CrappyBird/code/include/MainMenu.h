#pragma once
#include "AIngine/Core.h"
#include "AIngine/UI/Button.h"

namespace CrappyBird {
	class MainMenu : public AIngine::Script {
	public:
		MainMenu();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;
		virtual void OnGUI() override;

	private:
		void OnStartClicked();

	private:
		AIngine::UI::Button::OnClickedEventHandler OnStartClickedHandler;
		AIngine::UI::Button* StartButton;
	};
}