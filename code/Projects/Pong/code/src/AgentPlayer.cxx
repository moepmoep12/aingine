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
		constants.n = 1200;
		constants.beta = 0.1;
		constants.dontCareProbability = 0.4;
		constants.gamma = 0.6;
		constants.thetaGA = 50;
		constants.useMAM = true;
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
		static float minY = AIngine::World::GetBounds().z + GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;
		static float maxY = AIngine::World::GetBounds().w - GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;

		Player::Update(delta);

		currentTick++;
		if (currentTick >= tickRate) {
			currentTick = 0;

			if (m_HasBall)
				Player::StartBall();

			int action = explore ? m_xcsr->explore(situation()) : m_xcsr->exploit(situation(), true);
			if (action == 2) action = -1;
			if (action == 3) action = 0;
			lastActionTaken = action;

			if ((action == 1 && GetOwner()->GetWorldPosition().y == maxY)
				|| (action == 2 && GetOwner()->GetWorldPosition().y == minY)) {
				EndExperiment(0);
				return;
			}

			Move(action);

			if (scored)
			{
				if (goalie == this->Role) {
					EndExperiment(1000);
				}
				else {
					EndExperiment(0);
				}
				scored = false;
				return;
			}

			else if (m_rigidBody->GetContact() && m_rigidBody->GetContact()->Other->GetOwner()->GetComponent<Ball>()) {
				EndExperiment(1000);
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
		static std::vector<double> FitnessValues;
		static std::vector<double> AccuracyValues;
		static std::vector<double> EpsilonValues;
		static double accuracyAverage = 0;
		static double accuracyMax = 0;
		static double fitnessAverage = 0;
		static double epsilonAverage = 0;
		static double epsilonMax = 0;
		static double predictionAverage = 0;
		static int numerosity = 0;
		static bool open = true;

		guiTick++;

		bool update = guiTick >= guiTicks;

		if (!ImGui::Begin("Agent", &open, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::End();
			return;
		}

		auto xcsrCS = dynamic_cast<const xxr::xcsr_impl::csr::Experiment<double, int>*>(&m_xcsr->GetExperiment());
		if (xcsrCS) {
			auto population = xcsrCS->GetPopulation();

			if (update) {
				guiTick = 0;
				// reset values
				accuracyAverage = 0;
				fitnessAverage = 0;
				epsilonAverage = 0;
				predictionAverage = 0;
				numerosity = 0;
				FitnessValues.clear();
				AccuracyValues.clear();

				for (auto cl : population) {
					accuracyAverage += cl->accuracy();
					fitnessAverage += cl->fitness;
					epsilonAverage += cl->epsilon;
					predictionAverage += cl->prediction;
					numerosity += cl->numerosity;
					FitnessValues.push_back(cl->fitness);
					AccuracyValues.push_back(cl->accuracy());
					EpsilonValues.push_back(cl->epsilon);

					if (cl->epsilon > epsilonMax)
						epsilonMax = cl->epsilon;

					if (cl->accuracy() > accuracyMax)
						accuracyMax = cl->accuracy();
				}

				accuracyAverage /= population.size();
				fitnessAverage /= population.size();
				epsilonAverage /= population.size();
				predictionAverage /= population.size();

				std::sort(FitnessValues.begin(), FitnessValues.end());
				std::sort(AccuracyValues.begin(), AccuracyValues.end());
				std::sort(EpsilonValues.begin(), EpsilonValues.end());
			}


			// Draw FitnessAverage Graph
			{
				static std::vector<float> FitnessQueue;
				static float FitnessMax = 0;
				static float FitnessMin = 10;
				static const int maxElements = 2000;
				struct Funcs
				{
					static float Get(void*, int i) { return FitnessQueue[i]; }
				};
				float(*func)(void*, int) = Funcs::Get;

				if (FitnessQueue.size() >= maxElements) {
					FitnessQueue.erase(FitnessQueue.begin());
				}
				if (update)
				{
					FitnessQueue.push_back(fitnessAverage);
					if (fitnessAverage > FitnessMax)
						FitnessMax = fitnessAverage;
					if (fitnessAverage < FitnessMin) {
						FitnessMin = fitnessAverage;
					}
				}
				std::string t = std::to_string(fitnessAverage);
				ImGui::PlotLines("Fitness Average", func, NULL, FitnessQueue.size(), 0, t.c_str(), FitnessMin, FitnessMax, ImVec2(0, 150));
			}

			// Draw FitnessValues Histogramm
			{
				ImGui::SameLine();
				struct Funcs
				{
					static float Get(void*, int i) { return FitnessValues[i]; }
				};
				float(*func)(void*, int) = Funcs::Get;

				ImGui::PlotHistogram("Fitness Values", func, NULL, FitnessValues.size(), 0, NULL, 0, 1, ImVec2(0, 150));

			}

			ImGui::Separator();

			// Draw Epsilon-Average Graph
			{
				static std::vector<float> EpsilonQueue;
				static float EpsilonMax = 0;
				static float EpsilonMin = 10;
				static const int maxElements = 2000;
				struct Funcs
				{
					static float Get(void*, int i) { return EpsilonQueue[i]; }
				};
				float(*func)(void*, int) = Funcs::Get;

				if (EpsilonQueue.size() >= maxElements) {
					EpsilonQueue.erase(EpsilonQueue.begin());
				}

				if (update)
				{
					EpsilonQueue.push_back(epsilonAverage);
					if (epsilonAverage > EpsilonMax)
						EpsilonMax = epsilonAverage;
					if (epsilonAverage < EpsilonMin) {
						EpsilonMin = epsilonAverage;
					}
				}

				std::string t = std::to_string(epsilonAverage);
				ImGui::PlotLines("Epsilon Average", func, NULL, EpsilonQueue.size(), 0, t.c_str(), EpsilonMin, EpsilonMax, ImVec2(0, 150));
			}

			// Draw EpsilonValues Histogramm
			{
				ImGui::SameLine();
				struct Funcs
				{
					static float Get(void*, int i) { return EpsilonValues[i]; }
				};
				float(*func)(void*, int) = Funcs::Get;

				ImGui::PlotHistogram("Epsilon Values", func, NULL, EpsilonValues.size(), 0, NULL, 0, epsilonMax, ImVec2(0, 150));

			}

			ImGui::Separator();

			// Draw AccuracyAverage Graph
			{
				static std::vector<float> AccuracyQueue;
				static float AccuracyMax = 0;
				static float AccuracyMin = 10;
				static const int maxElements = 2000;
				struct Funcs
				{
					static float Get(void*, int i) { return AccuracyQueue[i]; }
				};
				float(*func)(void*, int) = Funcs::Get;

				if (AccuracyQueue.size() >= maxElements) {
					AccuracyQueue.erase(AccuracyQueue.begin());
				}

				if (update)
				{
					AccuracyQueue.push_back(accuracyAverage);
					if (accuracyAverage > AccuracyMax)
						AccuracyMax = accuracyAverage;
					if (accuracyAverage < AccuracyMin) {
						AccuracyMin = accuracyAverage;
					}
				}

				std::string t = std::to_string(accuracyAverage);
				ImGui::PlotLines("Accuracy Average", func, NULL, AccuracyQueue.size(), 0, t.c_str(), AccuracyMin, AccuracyMax, ImVec2(0, 150));
			}


			// Draw FitnessValues Histogramm
			{
				ImGui::SameLine();
				struct Funcs
				{
					static float Get(void*, int i) { return AccuracyValues[i]; }
				};
				float(*func)(void*, int) = Funcs::Get;

				ImGui::PlotHistogram("Accuracy Values", func, NULL, AccuracyValues.size(), 0, NULL, 0, accuracyMax, ImVec2(0, 150));

			}

			ImGui::NewLine();

			std::stringstream ss;
			ss << "Population Size : " << population.size();
			ImGui::Text(ss.str().c_str());

			ss.str(std::string());
			ss << "Numerosity Sum : " << numerosity;
			ImGui::Text(ss.str().c_str());

			ImGui::End();
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

		glm::vec2 worldPos = GetOwner()->GetWorldPosition();
		glm::vec2 ballPos = m_ball->GetOwner()->GetWorldPosition();

		std::vector<double> result =
		{
			worldPos.y / rect.height, // relative height
			glm::distance(worldPos, ballPos) / rect.width, // relative distance to ball
			m_BallBody->GetVelocity().x / 10.0, // relative ball velocity.x
			m_BallBody->GetVelocity().y / 10.0, // relative ball velocity.y
			ballPos.x / rect.width, // relative ballpos.x
			ballPos.y / rect.height, // relative ballpos.y
			(double)lastActionTaken// last action taken
		};

		return result;
	}
	void AgentPlayer::EndExperiment(float reward)
	{
		Player* other = Role == PlayerRole::One ? m_ball->PlayerTwo : m_ball->PlayerOne;
		other->ReceiveBall();
		m_xcsr->reward(reward, true);
	}
}