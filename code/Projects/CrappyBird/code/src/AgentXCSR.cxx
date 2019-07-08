#include "AgentXCSR.h"
#include "CrappyBird.h"
#include "Obstacle.h"
#include "Player.h"

namespace CrappyBird {

	// Constructor
	AgentXCSR::AgentXCSR()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
		m_originalGameSpeed = CrappyBird::s_GameSpeed;

	}

	// Start is called when gameplay starts for this script
	void AgentXCSR::OnStart()
	{
		CrappyBird::s_DieOnCollision = true;

		m_physBody = GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>();
		m_physBody->SetFixedRotation(true);
		m_physBody->SetEnabled(true);

		OnCollisionEventHandler = AIngine::Events::EventHandler<void, AIngine::Physics::Contact>(std::bind(&AgentXCSR::OnCollision, this, std::placeholders::_1));
		OnGameOverHandler = OnGameOverEventHandler(std::bind(&AgentXCSR::OnGameOver, this));

		// register callbacks
		m_physBody->OnCollisionBegin += OnCollisionEventHandler;
		OnGameOverEvent += OnGameOverHandler;

		static auto constants = xxr::XCSRConstants();
		constants.n = 300;
		static std::unordered_set<int> options = std::unordered_set<int>{ 0, 1 };
		m_xcsr = new xxr::XCSR<>(xxr::CSR, options, constants);
		//m_spawnPosition = GetOwner()->GetWorldPosition();
	}

	// End is called when gameplay ends for this script
	void AgentXCSR::OnEnd()
	{
		// unsubscribe from events
		m_physBody->OnCollisionBegin -= OnCollisionEventHandler;
		OnGameOverEvent -= OnGameOverHandler;
		delete m_xcsr;
	}

	// Update is called once per frame
	void AgentXCSR::Update(float delta)
	{
		// Check if GameOver
		if (GetOwner()->GetWorldPosition().x < 0) {
			OnGameOverEvent();
		}

		// update score
		m_distanceTraveled += CrappyBird::s_GameSpeed * delta;

		// check if new screen has been entered and fire event
		if (m_distanceTraveled >= CurrentScreenIndex * 10) {
			CurrentScreenIndex++;
			glm::vec4 bounds = AIngine::World::GetBounds();
			AIngine::Structures::RectangleF nextScreen(bounds.y, 0, bounds.y, bounds.w / 2.0f);
			OnEnterNewScreen(nextScreen);
		}

		bool action = m_xcsr->explore(situation());

		if (action) {
			// accelerate
			m_physBody->ApplyLinearImpulseToCenter(glm::vec2(0, -CrappyBird::s_Impulse));
		}

		if (m_physBody->GetContact()) 
		{
			m_xcsr->reward(-100, true);
			return;
		}

		m_xcsr->reward(0.1f, false);
	}

	// Callback for events
	void AgentXCSR::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void AgentXCSR::PostInit()
	{
		GetOwner()->SetActive(CrappyBird::s_AgentLearning);
	}

	void AgentXCSR::ResetGame()
	{
		m_distanceTraveled = 0;
		IsGameOver = false;
		m_physBody->SetEnabled(true);
		GetOwner()->SetWorldPosition(m_spawnPosition);
		CurrentScreenIndex = 1;

		GameObject* obstacleParent = AIngine::World::GetGameObject("Obstacles");
		for (auto& child : obstacleParent->GetChildren()) {
			child->SetWorldPosition(glm::vec2(-5, 0));
			child->SetActive(false);
		}
		GameObject* pickupParent = AIngine::World::GetGameObject("PickUpFactory");
		for (auto& child : pickupParent->GetChildren()) {
			child->SetActive(false);
		}

		OnRestartGame();

	}

	void AgentXCSR::OnGameOver()
	{
		IsGameOver = true;
		m_physBody->SetEnabled(false);
		ResetGame();
	}

	void AgentXCSR::OnCollision(AIngine::Physics::Contact contact)
	{
		if (CrappyBird::s_DieOnCollision) {
			if (contact.Other->GetOwner()->GetComponent<Obstacle>()) {
				OnGameOverEvent();
			}
		}
	}

	std::vector<double> AgentXCSR::situation()
	{
		return
		{
			GetOwner()->GetLocalPosition().y / 5.625,
			m_physBody->GetVelocity().length() / 10.0
		};
	}

	double AgentXCSR::executeAction(bool action)
	{
		return 0.0;
	}

	bool AgentXCSR::isEndOfProblem() const
	{
		return false;
	}
}