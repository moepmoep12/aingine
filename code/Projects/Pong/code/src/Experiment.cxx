#include "Experiment.h"
#include "HumanPlayer.h"
#include "Pong.h"
namespace Pong {

	// Constructor
	Experiment::Experiment()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());

		m_TrainingTicksPerDifficulty[0] = 2000;
		m_TrainingTicksPerDifficulty[1] = 3000;
		m_TrainingTicksPerDifficulty[2] = 4000;
		m_TrainingTicksPerDifficulty[3] = 5000;
	}

	// Start is called when gameplay starts for this script
	void Experiment::OnStart()
	{
		m_sound = GetComponent<SoundComponent>();

		LeftEdge = GetOwner()->GetChild("Edges")->GetChild("LeftEdge")->GetComponent<PhysicsComponent>();
		RightEdge = GetOwner()->GetChild("Edges")->GetChild("RightEdge")->GetComponent<PhysicsComponent>();
		TopEdge = GetOwner()->GetChild("Edges")->GetChild("TopEdge")->GetComponent<PhysicsComponent>();
		BottomEdge = GetOwner()->GetChild("Edges")->GetChild("BottomEdge")->GetComponent<PhysicsComponent>();

		LeftEdge->OnCollisionBegin += std::bind(&Experiment::OnCollisionLeft, this, std::placeholders::_1);
		RightEdge->OnCollisionBegin += std::bind(&Experiment::OnCollisionRight, this, std::placeholders::_1);

		ArenaRect = AIngine::Structures::RectangleF(GetOwner()->GetWorldPosition().x, GetOwner()->GetWorldPosition().y,
			TopEdge->GetBodyInformation().width, RightEdge->GetBodyInformation().height);

		ScoreText = AIngine::World::GetGameObject("ScoreText")->GetComponent<AIngine::UI::UIText>();
		TrainingText = AIngine::World::GetGameObject("TrainingText")->GetComponent<AIngine::UI::UIText>();

		auto components = GetOwner()->GetChild("PlayerOne")->GetComponents();
		for (auto& comp : components)
			if (dynamic_cast<Player*>(comp)) {
				PlayerOne = dynamic_cast<Player*>(comp);
				PlayerOne->Role = PlayerRole::One;
				break;
			}

		components = GetOwner()->GetChild("PlayerTwo")->GetComponents();
		for (auto& comp : components)
			if (dynamic_cast<Player*>(comp)) {
				PlayerTwo = dynamic_cast<Player*>(comp);
				PlayerTwo->Role = PlayerRole::Two;
				break;
			}

		BallBody = GetOwner()->GetChild("Ball")->GetComponent<PhysicsComponent>();
		PlayerOne->ReceiveBall();
		supervisor = AIngine::World::GetGameObject("AgentSupervisor")->GetComponent<AIngine::XCSAgentSupervisor>();

	}

	// End is called when gameplay ends for this script
	void Experiment::OnEnd()
	{
		LeftEdge->OnCollisionBegin = AIngine::Physics::CollisionEvent();
		RightEdge->OnCollisionBegin = AIngine::Physics::CollisionEvent();

		ScorePlayerOne = 0;
		ScorePlayerTwo = 0;
	}

	// Update is called once per frame
	void Experiment::Update(float delta)
	{
		if (m_training) {
			m_currentTrainingTicks++;
			if (m_currentTrainingTicks >= m_TrainingTicksPerDifficulty[Pong::Difficulty]) {
				m_training = false;
				// replace the random agent with the human player
				PlayerOne->GetOwner()->SetActive(false);

				PlayerOne = AIngine::World::GetGameObject("PlayerOneHuman")->GetComponent<HumanPlayer>();
				PlayerOne->GetComponent<PhysicsComponent>()->SetEnabled(true);
				PlayerOne->GetOwner()->SetActive(true);
				PlayerOne->ReceiveBall();
				ScoreText->GetOwner()->SetActive(true);
				TrainingText->GetOwner()->SetActive(false);


				ScorePlayerOne = 0;
				ScorePlayerTwo = 0;
				((Pong&)Pong::Get()).SetVsync(true);
				
			}
			else {
				if (m_currentTrainingTicks >= m_TrainingTicksPerDifficulty[Pong::Difficulty] * 0.5)
					if (supervisor->Exploration) {
						supervisor->Exploration = false;
					}
			}
		}
	}

	// Callback for events
	void Experiment::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void Experiment::OnGUI()
	{
		std::stringstream ss;
		if (m_training)
		{
			ss << "Training " << (int)(100.0f * ((float)m_currentTrainingTicks / (float)m_TrainingTicksPerDifficulty[Pong::Difficulty])) << "%";
			TrainingText->Text = ss.str();
		}
		else {
			ss << ScorePlayerOne << "  :  " << ScorePlayerTwo;
			ScoreText->Text = ss.str();
		}
		Graphics::BoxWorld(ArenaRect, glm::vec4(0, 0, 0, 1));
	}

	void Experiment::OnCollisionLeft(AIngine::Physics::Contact contact)
	{
		ScorePlayerTwo++;
		PlayerTwo->OnScored(PlayerRole::Two);
		PlayerOne->OnScored(PlayerRole::Two);
		if (m_training)
			PlayerOne->ReceiveBall();
		else
			PlayerTwo->ReceiveBall();
	}

	void Experiment::OnCollisionRight(AIngine::Physics::Contact contact)
	{
		ScorePlayerOne++;
		PlayerTwo->OnScored(PlayerRole::One);
		PlayerOne->OnScored(PlayerRole::One);
		PlayerOne->ReceiveBall();
#ifndef EDITOR
		if (!m_training)
			m_sound->Play(0);
#endif 

	}
}