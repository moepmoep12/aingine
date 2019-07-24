#include "XCSAgent.h"
#include "Experiment.h"
#include "imgui.h"
#include "AIngine/XCSAgents.h"
#include "AIngine/Util/Project.h"

#include <cmath>
#include <filesystem>

namespace Pong {

	// Constructor
	XCSAgent::XCSAgent()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
		TranslationRate = 15;
		m_populationDifficulties[0] = 2500;
		m_populationDifficulties[1] = 2500;
		m_populationDifficulties[2] = 2500;
	}

	// Start is called when gameplay starts for this script
	void XCSAgent::OnStart()
	{
		Player::OnStart();


		AIngine::XCSAgentSupervisor* supervisor = AIngine::World::GetGameObject("AgentSupervisor")->GetComponent<AIngine::XCSAgentSupervisor>();
#ifndef EDITOR
		supervisor->GetConstants().n = m_populationDifficulties[Pong::Difficulty];
		std::stringstream ss;
		ss << AIngine::Util::Project::GetResourceDirectory() << "xcs\\difficulty" << Pong::Difficulty << ".json";
		supervisor->m_xcsr->loadPopulationCSV(std::filesystem::absolute(ss.str()).string(), true);
		supervisor->SwitchCondensationMode(true);
		supervisor->Exploration = false;
#endif
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
#ifdef EDITOR
		glm::vec4 color = (collisionPointY >= m_experiment->ArenaRect.y && collisionPointY <= m_experiment->ArenaRect.GetMax().y)
			&& std::abs(distanceToCollisionPoint) < 0.5f ? glm::vec4(0, 1, 0, 1) : glm::vec4(0, 0, 0, 1);
		Graphics::Point(glm::vec2(collisionPointX, collisionPointY), 18, color);

		glm::vec2 position(10, 100);

		for (auto score : scores) {
			std::stringstream ss;
			float value = (float)score.second / (float)(score.first + score.second);

			ss << std::fixed << std::setprecision(1) << value * 100 << "%";
			Graphics::Text(ss.str(), position);
			position += glm::vec2(0, 50);
		}
#endif
	}

	void XCSAgent::OnWidget()
	{
		if (ImGui::Checkbox("Measure Performance", &testing)) {
			m_experiment->ScorePlayerOne = 0;
			m_experiment->ScorePlayerTwo = 0;
		}

		for (int i = 0; i < observation.size(); i++) {
			ImGui::InputDouble(std::to_string(i).c_str(), &observation[i]);
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

		if (rect.Contains(intersections[0]) && ballVel.x > 0) {
			collisionPointX = intersections[0].x;
			collisionPointY = intersections[0].y;
			distanceToCollisionPoint = relativePos.y - collisionPointY;
		}
		else {
			collisionPointY = rect.y - 1;
			distanceToCollisionPoint = -1;
		}

		std::vector<double> result = {
			/* 0 */ relativePos.y / rect.height, // agent height
			/* 2 */ std::abs(relativePos.x - ballPos.x) / rect.width, // distance to ball X
			/* 3 */	(relativePos.y - ballPos.y) / rect.height, // distance to ball Y
			/* 4 */ std::clamp(ballVel.x / 10.0, -1.0, 1.0), // ballVelocity X
			/* 5 */ std::clamp(ballVel.y / 10.0, -1.0, 1.0), // ballVelocity Y
			/* 6 */ ballPos.x / rect.width, //  ballPos X
			/* 7 */ ballPos.y / rect.height, //  ballPos Y
				  //std::clamp(collisionPointX / rect.width, -1.05, 1.05), //  collisionpoint X
				  ///* 8 */ std::clamp(collisionPointY / rect.height, -1.05, 1.05), //  collisionpoint Y
				  ///* 9 */ distanceToCollisionPoint != -1 ? distanceToCollisionPoint / rect.height : -1, // distance to collisionpoint Y
				  ///* 10 */ other->GetOwner()->GetLocalPosition().y / rect.height, // other player height
				  /* 11 */ (double)lastAction, // last action
				  /* 12 */ (rect.GetMax().y - ballPos.y) / rect.height, // ball distance to bottom edge
				  /* 13 */ (rect.y - ballPos.y) / rect.height, // ball distance to top edge
		};

		for (auto& point : intersections) {
			if (isnan(point.x) || isnan(point.y)) {
				result.push_back(-1);
				result.push_back(-1);
			}
			else {
				result.push_back(std::clamp(point.x / rect.width, -1.0f, 1.0f));
				result.push_back(std::clamp(point.y / rect.height, -1.0f, 1.0f));
			}
		}

		observation = result;
		return result;
	}

	bool XCSAgent::IsEndOfProblem()
	{
		if (scored) {
			scored = false;
			if (goalie == Role) {
				AddReward(1000);
			}
			return true;
		}

		return DoesBallCollide();
	}

	void XCSAgent::ExecuteAction(int action)
	{
		lastAction = action;

		Move(action);

		// Provide Reward

		if (DoesBallCollide()) {
#ifdef EDITOR
			m_experiment->ScorePlayerTwo++;
#endif
			AddReward(1000);
			return;
		}


		// collision out of reach
		if (collisionPointY < m_experiment->ArenaRect.y || collisionPointY > m_experiment->ArenaRect.GetMax().y) {
			if (action == 0) AddReward(10);
		}
		else {
			// we're already in the right spot
			if (std::abs(distanceToCollisionPoint) < 0.5f) {
				AddReward(20);
				// add more reward if the agent doesn't move once in the right spot
				if (action == 0) AddReward(50);
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
#ifdef EDITOR
		other->ReceiveBall();
#endif
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
		Player* other = Role == PlayerRole::One ? m_experiment->PlayerTwo : m_experiment->PlayerOne;
		double axisOther = other->GetOwner()->GetLocalPosition().x;

		std::vector<glm::vec2> intersections;

		float m = ballVel.y / ballVel.x;
		float b = ballPos.y - (m * ballPos.x);

		// player Axis
		intersections.push_back(glm::vec2(axis, m* axis + b));
		// other player
		intersections.push_back(glm::vec2(axisOther, m* axisOther + b));
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