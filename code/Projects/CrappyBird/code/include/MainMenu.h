#pragma once
#include "AIngine/Core.h"
#include "AIngine/UI/Button.h"
#include "AIngine/UI/Slider.h"
#include "AIngine/UI/CheckBox.h"

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
		void RotateObstacles(float delta);

		/* Callbacks */
		void OnStartClicked();
		void OnLevelButtonClicked();
		void OnSpeedChanged(float value);
		void OnImpulseChanged(float value);
		void OnObstacleRotationValueChanged(bool value);
		void OnGameOverCheckBoxValueChanged(bool value);

	private:

		AIngine::UI::Button::OnClickedEventHandler OnStartClickedHandler;
		AIngine::UI::Button* StartButton;

		AIngine::UI::Slider* SpeedSlider;
		AIngine::UI::Slider* ImpulseSlider;
		AIngine::UI::CheckBox* RotationCheckBox;
		AIngine::UI::CheckBox* CollisionCheckBox;

		GameObject* Obstacles[2];

		AIngine::UI::Button* LevelButtons[4];
		Texture2D BackGrounds[4];
	};
}