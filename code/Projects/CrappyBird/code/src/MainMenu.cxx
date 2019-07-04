#include "MainMenu.h"
#include "CrappyBird.h"

namespace CrappyBird {

	// Constructor
	MainMenu::MainMenu()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());

		for (int i = 1; i < 5; i++) {
			std::stringstream ss;
			ss << "textures/Background/" << i << "/" << i << "_game_background.png";
			BackGrounds[i-1] = Texture2D(Assets::Load<BitmapAsset>(ss.str())->GetBitmap());
		}
	}

	// Start is called when gameplay starts for this script
	void MainMenu::OnStart()
	{
		StartButton = AIngine::World::GetGameObject("StartButton")->GetComponent<AIngine::UI::Button>();
		OnStartClickedHandler = AIngine::UI::Button::OnClickedEventHandler(std::bind(&MainMenu::OnStartClicked, this));
		StartButton->OnClickedEvent += OnStartClickedHandler;

		for (int i = 0; i < 4; i++) {
			std::stringstream ss;
			ss << "Level" << i + 1 << "Button";
			LevelButtons[i] = AIngine::World::GetGameObject(ss.str())->GetComponent<AIngine::UI::Button>();
			LevelButtons[i]->OnClickedEvent += std::bind(&MainMenu::OnLevelButtonClicked, this);
		}
	}

	// End is called when gameplay ends for this script
	void MainMenu::OnEnd()
	{
		StartButton->OnClickedEvent -= OnStartClickedHandler;
		for (int i = 0; i < 4; i++) {
			LevelButtons[i]->OnClickedEvent = AIngine::Events::Event<void>();
		}
	}

	// Update is called once per frame
	void MainMenu::Update(float delta)
	{
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
			Graphics::BoxScreen(rect, glm::vec3(1, 1, 0));
		}
	}
	void MainMenu::OnStartClicked()
	{
		AIngine::Application::LoadScene(1);
	}

	void MainMenu::OnLevelButtonClicked()
	{
		for (int i = 0; i < 4; i++) {
			if (LevelButtons[i]->IsClicked()) {
				CrappyBird::s_levelIndex = i;
				AIngine::World::GetGameObject("BackGround")->GetComponent<Sprite>()->SetTexture(BackGrounds[i]);
			}
		}
	}
}