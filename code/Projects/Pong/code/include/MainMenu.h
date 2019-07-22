#pragma once
#include "AIngine/Core.h"
#include "UI/Button.h"
#include "UI/Image.h"

namespace Pong {
	class MainMenu : public AIngine::Script {
	public:
		using Button = AIngine::UI::Button;
		using Image = AIngine::UI::Image;

		MainMenu();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

		const glm::vec4 SelectedTintColor = glm::vec4(0.5, 0.5, 0.5, 1);

		void OnDifficultyButtonClicked();
		void OnStartClicked();
		void ResizePlayer(float delta);

	private:
		Button::OnClickedEventHandler OnDifficultyClickedHandler;
		Button* m_easyButton;
		Button* m_challengeButton;
		Button* m_unfairButton;
		Button* m_terminatorButton;
		Button* m_startButton;
		std::vector<Button*> m_DifficultyButtons;

		Image* m_playerOne;

		SoundComponent* m_sound;

		float m_currentDuration = 1;
		float m_duration = 1;
		float m_sizeDiff = 0;
		float m_originalHeight = 0;
	};
}