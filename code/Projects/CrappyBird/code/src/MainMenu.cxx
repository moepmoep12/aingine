#include "MainMenu.h"
#include "CrappyBird.h"
#include "Player.h"

namespace CrappyBird {

	// Constructor
	MainMenu::MainMenu()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());

		for (int i = 1; i < 5; i++) {
			std::stringstream ss;
			ss << "textures/Background/" << i << "/" << i << "_game_background.png";
			BackGrounds[i - 1] = Texture2D(Assets::Load<BitmapAsset>(ss.str())->GetBitmap());
		}
	}

	// Start is called when gameplay starts for this script
	void MainMenu::OnStart()
	{
		StartButton = AIngine::World::GetGameObject("StartButton")->GetComponent<AIngine::UI::Button>();
		OnStartClickedHandler = AIngine::UI::Button::OnClickedEventHandler(std::bind(&MainMenu::OnStartClicked, this));
		StartButton->OnClickedEvent += OnStartClickedHandler;

		SpeedSlider = AIngine::World::GetGameObject("SpeedSlider")->GetComponent<AIngine::UI::Slider>();
		SpeedSlider->OnValueChangedEvent += std::bind(&MainMenu::OnSpeedChanged, this, std::placeholders::_1);

		ImpulseSlider = AIngine::World::GetGameObject("ImpulseSlider")->GetComponent<AIngine::UI::Slider>();
		ImpulseSlider->OnValueChangedEvent += std::bind(&MainMenu::OnImpulseChanged, this, std::placeholders::_1);

		RotationCheckBox = AIngine::World::GetGameObject("CheckBoxRotation")->GetComponent<AIngine::UI::CheckBox>();
		RotationCheckBox->OnStateChangedEvent += std::bind(&MainMenu::OnObstacleRotationValueChanged, this, std::placeholders::_1);

		CollisionCheckBox = AIngine::World::GetGameObject("CheckBoxCollision")->GetComponent<AIngine::UI::CheckBox>();
		CollisionCheckBox->OnStateChangedEvent += std::bind(&MainMenu::OnGameOverCheckBoxValueChanged, this, std::placeholders::_1);

		Obstacles[0] = AIngine::World::GetGameObject("Obstacle1");
		Obstacles[1] = AIngine::World::GetGameObject("Obstacle2");

		for (int i = 0; i < 4; i++) {
			std::stringstream ss;
			ss << "Level" << i + 1 << "Button";
			LevelButtons[i] = AIngine::World::GetGameObject(ss.str())->GetComponent<AIngine::UI::Button>();
			LevelButtons[i]->OnClickedEvent += std::bind(&MainMenu::OnLevelButtonClicked, this);
		}

		//play music
		AIngine::World::GetGameObject("BackGround")->GetComponent<SoundComponent>()->Play(CrappyBird::s_levelIndex);
	}

	// End is called when gameplay ends for this script
	void MainMenu::OnEnd()
	{
		StartButton->OnClickedEvent -= OnStartClickedHandler;
		for (int i = 0; i < 4; i++) {
			LevelButtons[i]->OnClickedEvent = AIngine::Events::Event<void>();
		}

		SpeedSlider->OnValueChangedEvent = AIngine::Events::Event<void, float>();
		ImpulseSlider->OnValueChangedEvent = AIngine::Events::Event<void, float>();
		RotationCheckBox->OnStateChangedEvent = AIngine::Events::Event<void, bool>();
		CollisionCheckBox->OnStateChangedEvent = AIngine::Events::Event<void, bool>();
	}

	// Update is called once per frame
	void MainMenu::Update(float delta)
	{
		RotateObstacles(delta);
	}

	// Callback for events
	void MainMenu::OnEventData(AIngine::Events::EventData & e)
	{
	}
	void MainMenu::OnGUI()
	{
		AIngine::UI::Button* btn = LevelButtons[CrappyBird::s_levelIndex];
		for (int i = 0; i < 10; i++) {
			auto rect = btn->GetRectangle();
			rect.x -= i;
			rect.y -= i;
			rect.width += 2 * i;
			rect.height += 2 * i;
			Graphics::BoxScreen(rect, glm::vec4(1, 1, 0, 1));
		}
	}

	static float rotationDirection = 0;

	void MainMenu::RotateObstacles(float delta)
	{
		if (!rotationDirection) return;

		static const float angle1 = -23 * AIngine::D2R;
		static const float angle2 = -69 * AIngine::D2R;
		static const float Duration = 1;
		static float currentDuration = 0;

		currentDuration += delta * rotationDirection;

		if (currentDuration >= Duration && rotationDirection == 1) {
			currentDuration = Duration;
			rotationDirection = 0;
		}
		else if (currentDuration <= 0 && rotationDirection == -1) {
			currentDuration = 0;
			rotationDirection = 0;
		}

		float t = AIngine::Math::SmoothStep(currentDuration / Duration);

		Obstacles[0]->SetRotation(t * angle1);
		Obstacles[1]->SetRotation(t * angle2);
	}

	void MainMenu::OnStartClicked()
	{
		AIngine::Application::LoadScene(1);
	}

	void MainMenu::OnLevelButtonClicked()
	{
		for (int i = 0; i < 4; i++) {
			if (LevelButtons[i]->IsClicked()) {
				//stop music
				AIngine::World::GetGameObject("BackGround")->GetComponent<SoundComponent>()->Stop(CrappyBird::s_levelIndex);
				CrappyBird::s_levelIndex = i;
				AIngine::World::GetGameObject("BackGround")->GetComponent<Sprite>()->SetTexture(BackGrounds[i]);
				//play music
				AIngine::World::GetGameObject("BackGround")->GetComponent<SoundComponent>()->Play(CrappyBird::s_levelIndex);
			}
		}
	}

	void MainMenu::OnSpeedChanged(float value)
	{
		CrappyBird::s_GameSpeed = CrappyBird::s_originalGameSpeed + value;
	}

	void MainMenu::OnImpulseChanged(float value)
	{
		Player::s_Impulse = Player::s_standardImpulse * value;
	}

	void MainMenu::OnObstacleRotationValueChanged(bool value)
	{
		CrappyBird::s_bObstacleRotation = value;
		if (value)
			rotationDirection = 1;
		else
			rotationDirection = -1;
	}

	void MainMenu::OnGameOverCheckBoxValueChanged(bool value)
	{
		CrappyBird::s_DieOnCollision = value;
	}
}