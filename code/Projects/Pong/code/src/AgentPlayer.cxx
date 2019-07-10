#include "AgentPlayer.h"
#include "Pong.h"
#include "Util/FileSystem.h"

#include <iostream>
#include "imgui.h"
#include <sstream>

namespace Pong {

	// Constructor
	AgentPlayer::AgentPlayer()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());

		static auto constants = xxr::XCSRConstants();
		constants.n = 500;
		//constants.useMAM = true;
		constants.epsilonZero = 10;
		static std::unordered_set<int> options = std::unordered_set<int>{ 1, 2,3 };
		m_xcsr = new xxr::XCSR<>(xxr::CSR, options, constants);
	}

	AgentPlayer::~AgentPlayer()
	{
		delete m_xcsr;
	}

	// Start is called when gameplay starts for this script
	void AgentPlayer::OnStart()
	{
		Pong::FIXED_TIMESTEP = 1.0 / 120.0;
		Pong::UseFixedTimeStep(true);
		Player::OnStart();

		m_ball = AIngine::World::GetGameObject("Ball")->GetComponent<Ball>();
	}

	// End is called when gameplay ends for this script
	void AgentPlayer::OnEnd()
	{
	}

	// Update is called once per frame

	static const int tickRate = 2;

	void AgentPlayer::Update(float delta)
	{
		Player::Update(delta);

		currentTick++;
		if (currentTick >= tickRate) {
			currentTick = 0;

			if (m_HasBall)
				Player::StartBall();

			int action = explore ? m_xcsr->explore(situation()) : m_xcsr->exploit(situation(), true);
			if (action == 2) action = -1;
			if (action == 3) action = 0;
			MoveAgent(action);


			if (scored)
			{
				if (goalie == this->Role) {
					m_xcsr->reward(1000, true);
				}
				else {
					m_xcsr->reward(0.1, true);
				}
				scored = false;
				return;
			}
			else if (m_rigidBody->GetContact() && m_rigidBody->GetContact()->Other->GetOwner()->GetComponent<Ball>()) {
				m_xcsr->reward(50, false);
			}
			else {
				m_xcsr->reward(0.1, false);
			}
		}
	}

	// Callback for events
	void AgentPlayer::OnEventData(AIngine::Events::EventData & e)
	{
		// key pressed
		if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent::KeyPressedEventData)) {
			AIngine::Events::KeyPressedEvent::KeyPressedEventData pressedEvent = dynamic_cast<AIngine::Events::KeyPressedEvent::KeyPressedEventData&>(e);

			if (pressedEvent.GetKeyCode() == AIngine::KeyCode::R) {
				Pong::Get().bRender = !Pong::Get().bRender;
			}
		}
	}

	void AgentPlayer::OnGUI()
	{
	}

	void AgentPlayer::OnWidget()
	{
		std::stringstream label;

		label << "Explore?##" << static_cast<const void*>(this);
		ImGui::Checkbox(label.str().c_str(), &explore);
		float gamespeed = (float)(Pong::Get().AppSpeedMulitplier);

		if (ImGui::DragFloat("GameSpeed", &gamespeed, 0.1, 0.5f, 10.0f)) {
			Pong::Get().AppSpeedMulitplier = gamespeed;
		}

		label.str(std::string());
		label << "Population Size : " << m_xcsr->populationSize();
		ImGui::Text(label.str().c_str());

		label.str(std::string());
		label << "Save Population##" << static_cast<const void*>(this);
		if (ImGui::Button(label.str().c_str())) {
			static const char* filterList = "txt,json";
			std::string path;
			AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::SaveFile(filterList, &path, "");

			if (result == AIngine::Util::Filesystem::Result::OKAY)
			{
				std::ofstream file;
				file.open(path);
				if (file.fail()) return;
				std::ostringstream os;
				m_xcsr->dumpPopulation(os);
				std::string content = os.str();
				file << content;
				file.close();
			}
		}

		label.str(std::string());
		label << "Load Population##" << static_cast<const void*>(this);

		if (ImGui::Button(label.str().c_str())) {
			static const char* filterList = "txt,json";
			std::string path;
			AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::OpenFile(filterList, &path, "");

			if (result == AIngine::Util::Filesystem::Result::OKAY)
			{
				m_xcsr->loadPopulationCSV(path, true);
			}
		}

		//	ImGui::Separator();
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