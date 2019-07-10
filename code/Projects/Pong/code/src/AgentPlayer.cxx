#include "AgentPlayer.h"
#include "Pong.h"
#include "Util/FileSystem.h"

#include <iostream>
#include "imgui.h"
#include <sstream>
#include <algorithm>

namespace Pong {

	// Constructor
	AgentPlayer::AgentPlayer()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());

		static auto constants = xxr::XCSRConstants();
		constants.n = 800;
		//constants.useMAM = true;
		constants.epsilonZero = 10;
		constants.minValue = -1;
		constants.exploreProbability = 0.5;
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
		Player::OnEnd();
	}

	// Update is called once per frame

	static const int tickRate = 1;

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
			Move(action);


			if (scored)
			{
				if (goalie == this->Role) {
					m_xcsr->reward(1000, true);
				}
				else {
					m_xcsr->reward(0, true);
				}
				scored = false;
				return;
			}
			else if (m_rigidBody->GetContact() && m_rigidBody->GetContact()->Other->GetOwner()->GetComponent<Ball>()) {
				Player* other = Role == PlayerRole::One ? m_ball->PlayerTwo : m_ball->PlayerOne;
				other->ReceiveBall();
				m_xcsr->reward(1000, true);
			}
			else {
				m_xcsr->reward(0, false);
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
		auto xcsrCS = dynamic_cast<const xxr::xcsr_impl::csr::Experiment<double, int>*>(&m_xcsr->GetExperiment());
		if (xcsrCS) {
			auto pop = xcsrCS->GetPopulation();

			double accuracy = 0;
			double fitness = 0;
			double epsilon = 0;
			double prediction = 0;
			static std::vector<float> fitnessValues;
			static std::vector<float> accuracyValues;
			fitnessValues.clear();
			accuracyValues.clear();
			int numerosity = 0;

			for (auto cl : pop) {
				accuracy += cl->accuracy();
				fitness += cl->fitness;
				fitnessValues.push_back(cl->fitness);
				accuracyValues.push_back(cl->accuracy());
				epsilon += cl->epsilon;
				prediction += cl->prediction;
				numerosity += cl->numerosity;
			}

			accuracy /= pop.size();
			fitness /= pop.size();
			epsilon /= pop.size();
			prediction /= pop.size();
			std::sort(fitnessValues.begin(), fitnessValues.end());
			std::sort(accuracyValues.begin(), accuracyValues.end());

			static bool open = true;
			if (ImGui::Begin("Agent", &open, ImGuiWindowFlags_AlwaysAutoResize)) {

				{
					struct Funcs
					{
						static float Get(void*, int i) { return fitnessValues[i]; }
					};
					float(*func)(void*, int) = Funcs::Get;
					ImGui::PlotLines("FitnessValues", func, NULL, fitnessValues.size(), 0, NULL, 0, 1, ImVec2(0, 150));
				}

				{
					static std::vector<float> FitnessQueue;
					static float FitnessMax = 0;
					static float FitnessMin = 10;
					struct Funcs
					{
						static float Get(void*, int i) { return FitnessQueue[i]; }

					};

					if (FitnessQueue.size() >= 2000) {
						FitnessQueue.erase(FitnessQueue.begin());
					}
					if (fitness > FitnessMax)
						FitnessMax = fitness;
					if (fitness < FitnessMin) {
						FitnessMin = fitness;
					}
					FitnessQueue.push_back(fitness);
					float(*func)(void*, int) = Funcs::Get;

					std::string t = std::to_string(fitness);
					ImGui::PlotLines("Fitness Average", func, NULL, FitnessQueue.size(), 0, t.c_str(), FitnessMin, FitnessMax, ImVec2(0, 150));
				}

				{
					static std::vector<float> EpsilonQueue;
					static float EpsilonMax = 0;
					static float EpsilonMin = 10;
					struct Funcs
					{
						static float Get(void*, int i) { return EpsilonQueue[i]; }

					};

					if (EpsilonQueue.size() >= 2000) {
						EpsilonQueue.erase(EpsilonQueue.begin());
					}
					if (epsilon > EpsilonMax)
						EpsilonMax = epsilon;
					if (epsilon < EpsilonMin) {
						EpsilonMin = epsilon;
					}
					EpsilonQueue.push_back(epsilon);
					float(*func)(void*, int) = Funcs::Get;
					std::string t = std::to_string(epsilon);
					ImGui::PlotLines("Epsilon Average", func, NULL, EpsilonQueue.size(), 0, t.c_str(), EpsilonMin, EpsilonMax, ImVec2(0, 150));
				}

				{
					struct Funcs
					{
						static float Get(void*, int i) { return accuracyValues[i]; }
					};
					float(*func)(void*, int) = Funcs::Get;
					ImGui::PlotLines("AccuracyValues", func, NULL, accuracyValues.size(), 0, NULL, 0, 1, ImVec2(0, 150));
				}

				{
					static std::vector<float> AccuracyQueue;
					static float AccuracyMax = 0;
					static float AccuracyMin = 1;
					struct Funcs
					{
						static float Get(void*, int i) { return AccuracyQueue[i]; }

					};

					if (AccuracyQueue.size() >= 2000) {
						AccuracyQueue.erase(AccuracyQueue.begin());
					}
					if (accuracy > AccuracyMax)
						AccuracyMax = accuracy;
					if (accuracy < AccuracyMin) {
						AccuracyMin = accuracy;
					}
					AccuracyQueue.push_back(accuracy);
					float(*func)(void*, int) = Funcs::Get;
					std::string t = std::to_string(accuracy);
					ImGui::PlotLines("Accuracy Average", func, NULL, AccuracyQueue.size(), 0, t.c_str(), AccuracyMin, AccuracyMax, ImVec2(0, 150));
				}
				std::stringstream ss;
				ss << "Population Size : " << pop.size();
				ImGui::Text(ss.str().c_str());

				ss.str(std::string());
				ss << "Numerosity Sum : " << numerosity;
				ImGui::Text(ss.str().c_str());

				ImGui::End();
			}
		}
	}

	void AgentPlayer::OnWidget()
	{
		std::stringstream label;

		label << "Explore?##" << static_cast<const void*>(this);
		ImGui::Checkbox(label.str().c_str(), &explore);

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
		label.str(std::string());
		label << "Condensation Mode##" << static_cast<const void*>(this);

		if (ImGui::Button(label.str().c_str())) {
			m_xcsr->switchToCondensationMode();
		}

	}

	void AgentPlayer::OnScored(PlayerRole role)
	{
		goalie = role;
		scored = true;
	}

	std::vector<double> AgentPlayer::situation()
	{
		static auto rect = AIngine::World::GetWorldRect();
		Player* other = Role == PlayerRole::One ? m_ball->PlayerTwo : m_ball->PlayerOne;

		std::vector<double> result =
		{
			GetOwner()->GetWorldPosition().y / rect.height,
			//other->GetOwner()->GetWorldPosition().y / rect.height,
			m_BallBody->GetVelocity().x / 10.0,
			m_BallBody->GetVelocity().y / 10.0,
			m_ball->GetOwner()->GetWorldPosition().x / rect.width,
			m_ball->GetOwner()->GetWorldPosition().y / rect.height,
		};

		return result;
	}
}