#include "BackGround.h"
#include "CrappyBird.h"

namespace CrappyBird {

	// Constructor
	BackGround::BackGround()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void BackGround::OnStart()
	{
		SoundComponent* soundComp = m_owner->GetComponent<SoundComponent>();
		soundComp->Play(CrappyBird::s_levelIndex);
		m_player = AIngine::World::GetGameObject("PlayerRocket")->GetComponent<Player>();

		OnGameOverHandler = Player::OnGameOverEventHandler(std::bind(&BackGround::OnGameOver, this));
		OnRestartGameHandler = Player::OnRestartGameEventHandler(std::bind(&BackGround::OnRestartGame, this));

		m_player->OnGameOverEvent += OnGameOverHandler;
		m_player->OnRestartGame += OnRestartGameHandler;

		auto children = GetOwner()->GetChildren();
		int j = 6;
		for (int i = 0; i < 12; i++) {
			std::stringstream ss;
			ss << "textures/Background/" << CrappyBird::s_levelIndex + 1 << "/" << j << ".png";
			children[i]->GetComponent<Sprite>()->SetTexture(Texture2D(Assets::Load<BitmapAsset>(ss.str())->GetBitmap()));
			if ((i + 1) % 2 == 0) {
				j--;
			}
		}
	}

	// End is called when gameplay ends for this script
	void BackGround::OnEnd()
	{
		SoundComponent* soundComp = m_owner->GetComponent<SoundComponent>();
		soundComp->Stop(CrappyBird::s_levelIndex);

		m_player->OnGameOverEvent -= OnGameOverHandler;
		m_player->OnRestartGame -= OnRestartGameHandler;
	}

	// Update is called once per frame
	void BackGround::Update(float delta)
	{
		if (m_player->IsGameOver) return;
		static const glm::vec4 bounds = AIngine::World::GetBounds();
		float i = 0.0f;
		int k = 1;
		for (auto& it : m_owner->GetChildren()) {
			it->Translate(glm::vec2(-CrappyBird::s_GameSpeed * i *delta, 0), false);
			glm::vec2 textureSize = it->GetComponent<AIngine::Sprite>()->GetLocalWorldSize();
			glm::vec worldPos = it->GetWorldPosition();
			if (worldPos.x <= bounds.x - 0.5f * textureSize.x) {
				it->SetWorldPosition(glm::vec2(bounds.y + 0.5f * textureSize.x, worldPos.y), false);
			}
			if (k == 2) {
				i += 0.2f;
				k = 0;
			}
			k++;
		}
	}

	// Callback for events
	void BackGround::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void BackGround::OnGameOver()
	{
		SoundComponent* soundComp = m_owner->GetComponent<SoundComponent>();
		soundComp->Stop(CrappyBird::s_levelIndex);
		soundComp->Play(4);
	}

	void BackGround::OnRestartGame()
	{
		SoundComponent* soundComp = m_owner->GetComponent<SoundComponent>();
		soundComp->Stop(4);
		soundComp->Play(CrappyBird::s_levelIndex);
	}
}