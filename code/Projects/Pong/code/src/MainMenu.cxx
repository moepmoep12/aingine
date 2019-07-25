#include "MainMenu.h"
#include "Pong.h"

namespace Pong {

	// Constructor
	MainMenu::MainMenu()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void MainMenu::OnStart()
	{
		GameObject* DifficultyParent = GetChild("Difficulty");
		OnDifficultyClickedHandler = Button::OnClickedEventHandler(std::bind(&MainMenu::OnDifficultyButtonClicked, this));

		m_easyButton = DifficultyParent->GetChild("Easy")->GetComponent<Button>();
		m_easyButton->OnClickedEvent += OnDifficultyClickedHandler;
		m_DifficultyButtons.push_back(m_easyButton);

		m_challengeButton = DifficultyParent->GetChild("Challenge")->GetComponent<Button>();
		m_challengeButton->OnClickedEvent += OnDifficultyClickedHandler;
		m_DifficultyButtons.push_back(m_challengeButton);

		m_unfairButton = DifficultyParent->GetChild("Unfair")->GetComponent<Button>();
		m_unfairButton->OnClickedEvent += OnDifficultyClickedHandler;
		m_DifficultyButtons.push_back(m_unfairButton);

		m_terminatorButton = DifficultyParent->GetChild("Terminator")->GetComponent<Button>();
		m_terminatorButton->OnClickedEvent += OnDifficultyClickedHandler;
		m_DifficultyButtons.push_back(m_terminatorButton);

		m_startButton = GetChild("StartButton")->GetComponent<Button>();
		m_startButton->OnClickedEvent += std::bind(&MainMenu::OnStartClicked, this);

		m_sound = GetChild("BackGround")->GetComponent<SoundComponent>();
		m_sound->Play(0);

		m_playerOne = GetChild("BackGround")->GetChild("PaddleOne")->GetComponent<Image>();
		m_playerTwo = GetChild("BackGround")->GetChild("PaddleTwo")->GetComponent<Image>();


		m_DifficultyButtons[Pong::Difficulty]->TintColor = SelectedTintColor;
		m_DifficultyButtons[Pong::Difficulty]->HoverColor = glm::vec4(0.6, 0.6, 0.6, 1);

		float TargetSize = 200 * Pong::PlayerSizes[Pong::Difficulty];
		m_sizeDiff = TargetSize - m_originalHeight;
		m_playerOne->SetHeight(m_originalHeight + m_sizeDiff);

		TargetSize = 200 * Pong::PlayerTwoSizes[Pong::Difficulty];
		m_sizeDiffTwo = TargetSize - m_originalHeightTwo;
		m_playerTwo->SetHeight(m_originalHeightTwo + m_sizeDiffTwo);
	}

	// End is called when gameplay ends for this script
	void MainMenu::OnEnd()
	{
		m_easyButton->OnClickedEvent -= OnDifficultyClickedHandler;
		m_challengeButton->OnClickedEvent -= OnDifficultyClickedHandler;
		m_unfairButton->OnClickedEvent -= OnDifficultyClickedHandler;
		m_terminatorButton->OnClickedEvent -= OnDifficultyClickedHandler;
		m_startButton->OnClickedEvent = AIngine::Events::Event<void>();
		m_DifficultyButtons.clear();

		m_sound->Stop(0);
	}

	// Update is called once per frame
	void MainMenu::Update(float delta)
	{
		if (m_currentDuration < m_duration)
			ResizePlayer(delta);
	}

	// Callback for events
	void MainMenu::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void MainMenu::OnDifficultyButtonClicked()
	{
		m_sound->Play(1);

		m_currentDuration = 0;

		m_DifficultyButtons[Pong::Difficulty]->TintColor = glm::vec4(1);
		m_DifficultyButtons[Pong::Difficulty]->HoverColor = glm::vec4(0.9, 0.9, 0.9, 1);


		if (m_easyButton->IsClicked()) {
			Pong::Difficulty = 0;
		}
		if (m_challengeButton->IsClicked()) {
			Pong::Difficulty = 1;
		}
		if (m_unfairButton->IsClicked()) {
			Pong::Difficulty = 2;
		}
		if (m_terminatorButton->IsClicked()) {
			Pong::Difficulty = 3;
		}

		m_DifficultyButtons[Pong::Difficulty]->TintColor = SelectedTintColor;
		m_DifficultyButtons[Pong::Difficulty]->HoverColor = glm::vec4(0.6, 0.6, 0.6, 1);

		Pong::Get().AppSpeedMulitplier = Pong::DifficultiesSpeed[Pong::Difficulty];

		m_originalHeight = m_playerOne->GetRectangleNative().height;
		m_originalHeightTwo = m_playerTwo->GetRectangleNative().height;
		float TargetSize = 200 * Pong::PlayerSizes[Pong::Difficulty];
		m_sizeDiff = TargetSize - m_originalHeight;

		TargetSize = 200 * Pong::PlayerTwoSizes[Pong::Difficulty];
		m_sizeDiffTwo = TargetSize - m_originalHeightTwo;
	}

	void MainMenu::OnStartClicked()
	{
		m_sound->Play(1);
		Pong::LoadScene(1);
	}

	void MainMenu::ResizePlayer(float delta)
	{
		m_currentDuration += delta;
		if (m_currentDuration >= m_duration) {
			m_currentDuration = m_duration;
		}
		float t = AIngine::Math::SmoothStep(m_currentDuration / m_duration);
		m_playerOne->SetHeight(m_originalHeight + t * m_sizeDiff);
		m_playerTwo->SetHeight(m_originalHeightTwo + t * m_sizeDiffTwo);
	}
}