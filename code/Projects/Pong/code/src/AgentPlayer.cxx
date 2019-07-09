#include "AgentPlayer.h"
#include "Pong.h"

namespace Pong {

	// Constructor
	AgentPlayer::AgentPlayer()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void AgentPlayer::OnStart()
	{
		Player::OnStart();

		m_ball = AIngine::World::GetGameObject("Ball")->GetComponent<Ball>();

		static auto constants = xxr::XCSRConstants();
		constants.n = 300;
		constants.useMAM = true;
		constants.epsilonZero = 10;
		static std::unordered_set<int> options = std::unordered_set<int>{ 1, 2 };
		m_xcsr = new xxr::XCSR<>(xxr::CSR, options, constants);
	}

	// End is called when gameplay ends for this script
	void AgentPlayer::OnEnd()
	{
		delete m_xcsr;
	}

	// Update is called once per frame

	static const int tickRate = 60;

	void AgentPlayer::Update(float delta)
	{
		Player::Update(delta);

		currentTick++;
		if (currentTick >= tickRate) {
			currentTick = 0;

			if (m_HasBall)
				Player::StartBall();

			int action = m_xcsr->explore(situation());
			if (action == 2) action = -1;
			MoveAgent(action);


			if (scored)
			{
				if (goalie == this->Role) {
					m_xcsr->reward(1000, true);
				}
				else {
					m_xcsr->reward(-1000, true);
				}
				scored = false;
				return;
			}
			else if (m_rigidBody->GetContact() && m_rigidBody->GetContact()->Other->GetOwner()->GetComponent<Ball>()) {
				m_xcsr->reward(100, false);
			}
			else {
				m_xcsr->reward(0.2, false);
			}
		}
	}

	// Callback for events
	void AgentPlayer::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void AgentPlayer::OnScored(PlayerRole role)
	{
		goalie = role;
		scored = true;
	}

	std::vector<double> AgentPlayer::situation()
	{
		static auto rect = AIngine::World::GetWorldRect();
		static Player* other = Role == PlayerRole::One ? m_ball->PlayerTwo : m_ball->PlayerOne;

		std::vector<double> result =
		{
			GetOwner()->GetWorldPosition().y / rect.height,
			other->GetOwner()->GetWorldPosition().y / rect.height,
			m_BallBody->GetVelocity().x / 10.0,
			m_BallBody->GetVelocity().y / 10.0,
			m_ball->GetOwner()->GetWorldPosition().x / rect.width,
			m_ball->GetOwner()->GetWorldPosition().y / rect.height,
		};

		return result;
	}

	void AgentPlayer::MoveAgent(int direction)
	{
		static float minY = AIngine::World::GetBounds().z + GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;
		static float maxY = AIngine::World::GetBounds().w - GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;
		float del = 0;
		float currentHeight = GetOwner()->GetWorldPosition().y;
		del = TranslationRate * direction * Pong::Get().GetDeltaTime();

		if (currentHeight + del > maxY) {
			del = maxY - currentHeight;
		}
		else if (currentHeight + del < minY) {
			del = minY - currentHeight;
		}

		m_rigidBody->GetOwner()->Translate(glm::vec2(0, del));
	}
}