#include "XCSAgent.h"
#include "Experiment.h"
#include "imgui.h"
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
		scores.clear();
	}

	// Update is called once per frame
	void XCSAgent::Update(float delta)
	{
		Player::Update(delta);
		// Fire the ball immediately
		if (m_HasBall) {
			StartBall();
		}

		if (testing && m_experiment->ScorePlayerOne + m_experiment->ScorePlayerTwo >= 300) {
			scores.push_back({ m_experiment->ScorePlayerOne ,m_experiment->ScorePlayerTwo });
			m_experiment->ScorePlayerOne = 0;
			m_experiment->ScorePlayerTwo = 0;
		}
	}

	void XCSAgent::OnGUI()
	{
		glm::vec4 color = collisionPointX == GetOwner()->GetLocalPosition().x  && std::abs(distanceToCollisionPoint) < 0.3f ? glm::vec4(0, 1, 0, 1) : glm::vec4(0, 0, 0, 1);
		Graphics::Point(glm::vec2(collisionPointX, collisionPointY), 18, color);

		glm::vec2 position(10, 100);

		for (auto score : scores) {
			std::stringstream ss;
			float value = (float)score.second / (float)(score.first + score.second);

			ss << std::fixed << std::setprecision(1) << value * 100 << "%";
			Graphics::Text(ss.str(), position);
			position += glm::vec2(0, 42);
		}
	}

	void XCSAgent::OnWidget()
	{
		if (ImGui::Checkbox("Measure Performance", &testing)) {
			m_experiment->ScorePlayerOne = 0;
			m_experiment->ScorePlayerTwo = 0;
		}
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
		glm::vec2 ballVel = m_experiment->BallBody->GetVelocity();
		std::vector<glm::vec2> intersections = CalcIntersections();
		int index = 0;

		if (rect.Contains(intersections[0]))
			index = 0;
		else if (ballVel.y < 0 && rect.Contains(intersections[1]))
			index = 1;
		else index = 2;

		collisionPointX = intersections[index].x;
		collisionPointY = intersections[index].y;

		if (ballVel.y == 0) collisionPointX = 0;

		if (collisionPointX == relativePos.x)
			distanceToCollisionPoint = std::abs(relativePos.y - collisionPointY);
		else
			distanceToCollisionPoint = -1;


		std::vector<double> result = {
			relativePos.y / rect.height, // agent height
			glm::distance(relativePos, ballPos) / rect.width, // distance to ball
			std::abs(relativePos.x - ballPos.x) / rect.width, // distance to ball X
			std::abs(relativePos.y - ballPos.y) / rect.height, // distance to ball X
			std::clamp(ballVel.x / 10.0, -1.0, 1.0), // ballVelocity X
			std::clamp(ballVel.y / 10.0, -1.0, 1.0), // ballVelocity Y
			ballPos.x / rect.width, //  ballPos Y
			ballPos.y / rect.height, //  ballPos X
			std::clamp(collisionPointX / rect.width, -1.05, 1.05), //  collisionpoint X
			std::clamp(collisionPointY / rect.height, -1.05, 1.05), //  collisionpoint Y
			std::clamp(distanceToCollisionPoint / rect.height, -1.05, 1.05), // distance to collisionpoint Y
			other->GetOwner()->GetLocalPosition().y / rect.height, // other player height
			(double)lastAction, // last action
			(rect.GetMax().y - ballPos.y) / rect.height, // ball distance to bottom edge
			(rect.y - ballPos.y) / rect.height, // ball distance to top edge
			//glm::dot(glm::vec2(1,0), glm::normalize(m_experiment->BallBody->GetVelocity()))
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
			m_experiment->ScorePlayerTwo++;
			return;
		}
		if (scored) {
			scored = false;
			if (goalie == Role)
				AddReward(1000);
			else
				AddReward(0);
			return;
		}

		// collision out of reach
		if (collisionPointX != GetOwner()->GetLocalPosition().x) {
			if (action == 0) AddReward(10);
		}
		else {
			// we're already in the right spot
			if (std::abs(distanceToCollisionPoint) < 0.3f) {
				AddReward(10);
				// add more reward if the agent doesn't move once in the right spot
				if (action == 0) AddReward(10);
			}
			else
			{
				// we should move up
				if (GetOwner()->GetLocalPosition().y > collisionPointY) {
					if (action == -1) AddReward(10);
				}
				// we should move down
				else if (action == 1) AddReward(10);
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

	std::vector<glm::vec2> XCSAgent::CalcIntersections()
	{
		static auto rect = m_experiment->ArenaRect;
		glm::vec2 ballPos = m_experiment->BallBody->GetOwner()->GetLocalPosition();
		glm::vec2 ballVel = m_experiment->BallBody->GetVelocity();
		double axis = GetOwner()->GetLocalPosition().x;

		std::vector<glm::vec2> intersections;

		float m = ballVel.y / ballVel.x;
		float b = ballPos.y - (m * ballPos.x);

		// player Axis
		intersections.push_back(glm::vec2(axis, m* axis + b));
		// top axis
		intersections.push_back(glm::vec2((rect.y - b) / m, rect.y));
		// bottom axis
		intersections.push_back(glm::vec2((rect.GetMax().y - b) / m, rect.GetMax().y));

		return intersections;
	}

	bool XCSAgent::DoesBallCollide()
	{
		return m_rigidBody->GetContact() && m_rigidBody->GetContact()->Other->GetOwner()->GetName() == "Ball";
	}
}