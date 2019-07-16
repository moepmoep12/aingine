#include "XCSAgent.h"
#include "Experiment.h"

namespace Pong {

	// Constructor
	XCSAgent::XCSAgent()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
		TranslationRate = 15;
	}

	// Start is called when gameplay starts for this script
	void XCSAgent::OnStart()
	{
		Player::OnStart();
	}

	// End is called when gameplay ends for this script
	void XCSAgent::OnEnd()
	{
		m_rigidBody->OnCollisionBegin = AIngine::Physics::CollisionEvent();
	}

	// Update is called once per frame
	void XCSAgent::Update(float delta)
	{
		Player::Update(delta);
		// Fire the ball immediately
		if (m_HasBall) {
			StartBall();
		}
	}

	void XCSAgent::OnGUI()
	{
		Graphics::Point(glm::vec2(GetOwner()->GetWorldPosition().x, collisionPointY), 10, glm::vec4(0, 0, 0, 1));
	}

	void XCSAgent::OnScored(PlayerRole goalie)
	{
		scored = true;
		this->goalie = goalie;
	}


	std::vector<double, std::allocator<double>> XCSAgent::Observe()
	{

		static auto rect = m_experiment->ArenaRect;
		Player* other = Role == PlayerRole::One ? m_experiment->PlayerTwo : m_experiment->PlayerOne;

		glm::vec2 relativePos = GetOwner()->GetLocalPosition();
		glm::vec2 ballPos = m_experiment->BallBody->GetOwner()->GetLocalPosition();
		collisionPointY = ComputeBallCollisionPoint();
		distanceToCollisionPoint = 0;

		if (collisionPointY < rect.y || collisionPointY > rect.GetMax().y)
			collisionPointY = m_experiment->ArenaRect.GetCenter().y;
		else {
			distanceToCollisionPoint = (relativePos.y - collisionPointY);
		}

		std::vector<double> result = {
			relativePos.y / rect.height,
			glm::distance(relativePos, ballPos) / rect.width,
			std::clamp(m_experiment->BallBody->GetVelocity().x / 10.0, -1.0, 1.0),
			std::clamp(m_experiment->BallBody->GetVelocity().y / 10.0, -1.0, 1.0),
			ballPos.x / rect.width,
			ballPos.y / rect.height,
			std::clamp(collisionPointY / rect.height, -1.0, 1.0),
			std::clamp(distanceToCollisionPoint / rect.height, -1.0, 1.0),
			other->GetOwner()->GetLocalPosition().y / rect.height,
			(double)lastAction
		};

		return result;
	}

	bool XCSAgent::IsEndOfProblem()
	{
		bool result = DoesBallCollide() || scored;
		return result;
	}

	void XCSAgent::ExecuteAction(int action)
	{
		lastAction = action;

		Move(action);

		// Provide Reward

		if (DoesBallCollide()) {
			AddReward(1000);
			return;
		}
		if (scored) {
			scored = false;
			if (goalie == Role)
				AddReward(1000);
			return;
		}

		// collision out of reach
		if (collisionPointY == m_experiment->ArenaRect.GetCenter().y) {

		}
		else {
			// we're already in the right spot
			if (std::abs(distanceToCollisionPoint) < 0.2f) {
				AddReward(10);
				// add more reward if the agent doesn't move once in the right spot
				if (action == 0) AddReward(1);
			}
			else
			{
				// we should move up
				if (GetOwner()->GetLocalPosition().y > collisionPointY) {
					if (action == -1) AddReward(1);
				}
				// we should move down
				else if (action == 1) AddReward(1);
			}
		}
	}

	void XCSAgent::Reset()
	{
		Player* other = Role == PlayerRole::One ? m_experiment->PlayerTwo : m_experiment->PlayerOne;
		other->ReceiveBall();
	}

	void XCSAgent::OnMaxStepsReached()
	{
	}

	double XCSAgent::ComputeBallCollisionPoint()
	{
		glm::vec2 ballPos = m_experiment->BallBody->GetOwner()->GetLocalPosition();
		glm::vec2 ballVel = m_experiment->BallBody->GetVelocity();
		double axis = GetOwner()->GetLocalPosition().x;

		/* y = v.y/v.x * x + (p.y - v.y/v.x * p.x)  */
		if (ballVel.x == 0) return m_experiment->ArenaRect.y - 1;;
		return (ballVel.y / ballVel.x) * axis + (ballPos.y - (ballVel.y / ballVel.x) * ballPos.x);
	}

	bool XCSAgent::DoesBallCollide()
	{
		return m_rigidBody->GetContact() && m_rigidBody->GetContact()->Other->GetOwner()->GetName() == "Ball";
	}
}