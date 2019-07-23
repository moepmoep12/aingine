#include "AgentXCSR.h"
#include "CrappyBird.h"
#include "Obstacle.h"
#include "Obstacles.h"
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
		//CrappyBird::FIXED_TIMESTEP = 1.0 / 120.0;
		//CrappyBird::UseFixedTimeStep(true);

		m_physBody = GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>();
		m_physBody->SetFixedRotation(true);
		m_physBody->SetEnabled(true);

		OnCollisionEventHandler = AIngine::Events::EventHandler<void, AIngine::Physics::Contact>(BIND_FN_1(AgentXCSR::OnCollision));
		OnGameOverHandler = OnGameOverEventHandler(BIND_FN_0(AgentXCSR::OnGameOver));

		// register callbacks
		m_physBody->OnCollisionBegin += OnCollisionEventHandler;
		OnGameOverEvent += OnGameOverHandler;

		static auto constants = xxr::XCSRConstants();
		constants.n = 300;
		constants.epsilonZero = 10;
		static std::unordered_set<int> options = std::unordered_set<int>{ 0, 1 };
		m_xcsr = new xxr::XCSR<>(options, constants, xxr::CSR);
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
		if (IsGameOver) return;
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
		else {
			m_physBody->ApplyLinearImpulseToCenter(glm::vec2(0, 0.2 * CrappyBird::s_Impulse));
		}

		if (m_distanceTraveled > 200) {
			m_xcsr->reward(1000, true);
			ResetGame();
			return;
		}
		else if (m_physBody->GetContact() && m_physBody->GetContact()->Other->GetOwner()->GetComponent<Obstacle>()) {
			OnGameOverEvent();
			return;
		}
		else {
			m_xcsr->reward(1, false);
		}
	}

	// Callback for events
	void AgentXCSR::OnEventData(AIngine::Events::EventData & e)
	{
		// key pressed
		if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent::KeyPressedEventData)) {
			AIngine::Events::KeyPressedEvent::KeyPressedEventData pressedEvent = dynamic_cast<AIngine::Events::KeyPressedEvent::KeyPressedEventData&>(e);

			if (pressedEvent.GetKeyCode() == AIngine::KeyCode::R) {
				CrappyBird::Get().bRender = false;
			}
		}
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
		m_xcsr->reward(-500 + m_distanceTraveled, true);
		ResetGame();
	}

	void AgentXCSR::OnCollision(AIngine::Physics::Contact contact)
	{

	}

	std::vector<double> AgentXCSR::situation()
	{
		std::vector<double> result =
		{
			GetOwner()->GetLocalPosition().y / 5.625,
			CrappyBird::s_GameSpeed / 3.5,
			m_physBody->GetVelocity().y / 10.0
		};

		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				if (!Obstacles::s_CurrentMap.empty()) {
					if (Obstacles::s_CurrentMap[i][j].isClosed) {
						result.push_back(Obstacles::s_CurrentMap[i][j].rectangle.x / 10.0);
						result.push_back(Obstacles::s_CurrentMap[i][j].rectangle.y / 5.625);
						result.push_back(Obstacles::s_CurrentMap[i][j].rectangle.width / 10.0);
						result.push_back(Obstacles::s_CurrentMap[i][j].rectangle.height / 5.625);
					}
					else {
						result.push_back(1);
						result.push_back(1);
						result.push_back(1);
						result.push_back(1);
					}
				}
				else {
					result.push_back(0);
					result.push_back(0);
					result.push_back(0);
					result.push_back(0);
				}
			}
		}

		return result;
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