#include "Agent.h"
#include <ostream>
#include "imgui.h"
namespace CrappyBird {

	static glm::vec2 s_spawnPos;

	// Constructor
	Agent::Agent()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void Agent::OnStart()
	{
		m_physBody = GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>();
		m_physBody->SetFixedRotation(true);

		static auto constants = xxr::XCSRConstants();
		constants.n = 300;
		static std::unordered_set<int> options = std::unordered_set<int>{ 0, 1 };
		xcsr = new xxr::XCSR<>(xxr::CSR, options, constants);
		s_spawnPos = GetOwner()->GetWorldPosition();

		GameObject* obstacleParent = AIngine::World::GetGameObject("Obstacles");
		for (auto& child : obstacleParent->GetChildren()) {
			obstacles.push_back(child);
		}
	}

	// End is called when gameplay ends for this script
	void Agent::OnEnd()
	{
		delete xcsr;
		obstacles.clear();
	}

	// Update is called once per frame
	void Agent::Update(float delta)
	{
		static const double maxWidth = 20;
		static const double maxHeight = 5;
		float currentheight = GetOwner()->GetWorldPosition().y;
		float normalizedHeight = currentheight / 5.0f;
		float vel = m_physBody->GetVelocity().y / 10.0f;

		std::vector<double> observations = { normalizedHeight,vel };

		//for (auto& obstacle : obstacles)
		//{
		//	glm::vec2 point = obstacle->GetWorldPosition();
		//	observations.push_back(point.x / maxWidth);
		//	observations.push_back(point.y / maxHeight);
		//	Sprite* sprite = obstacle->GetComponent<Sprite>();
		//	observations.push_back(sprite->GetLocalWorldSize().x / maxWidth);
		//	observations.push_back(sprite->GetLocalWorldSize().y / maxHeight);
		//}

		int action = xcsr->explore(observations);

		if (action == 1) {
			m_physBody->ApplyLinearImpulseToCenter(glm::vec2(0, -0.055f));
		}

		if (m_physBody->IsCollided() && m_physBody->GetOtherCollider()->GetOwner()->GetName().find("PickUp") == std::string::npos) {
			xcsr->reward(-1, true);
			GetOwner()->SetWorldPosition(s_spawnPos);
		}
		else {
			xcsr->reward(-std::abs(currentheight - s_spawnPos.y) + 0.1);
		}
	}

	// Callback for events
	void Agent::OnEventData(AIngine::Events::EventData & e)
	{
		// key pressed
		if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent::KeyPressedEventData)) {
			AIngine::Events::KeyPressedEvent::KeyPressedEventData pressedEvent = dynamic_cast<AIngine::Events::KeyPressedEvent::KeyPressedEventData&>(e);
			int i = 0;
			if (pressedEvent.GetKeyCode() == AIngine::KeyCodes::SPACE) {

			}
		}
	}

	void Agent::OnGUI()
	{
		std::ostringstream os;
		xcsr->dumpPopulation(os);
		std::string content = os.str();
		std::string line;
		std::stringstream ss(content);

		std::vector<float> predictions, epsilons, fitnesses, accuracies;
		int i = 0;

		while (std::getline(ss, line)) {
			if (i < 1) {
				i++;
				continue;
			}
			float prediction = 0;
			float epsilon = 0;
			float fitness = 0;
			float accuracy = 0;

			int start = line.find(',');
			start = line.find(',', start + 1);
			start = line.find(',', start);
			int end = line.find(',', start + 1);
			std::string sub = line.substr(start + 1, end - start - 1);

			prediction = std::stof(sub);

			start = end + 1;
			end = line.find(',', end + 1);
			sub = line.substr(start, end - start);

			epsilon = std::stof(sub);

			start = end + 1;
			end = line.find(',', end + 1);
			sub = line.substr(start, end - start);

			fitness = std::stof(sub);

			start = line.find_last_of(',');
			end = line.size();
			sub = line.substr(start + 1, end - start - 1);

			accuracy = std::stof(sub);

			predictions.push_back(prediction);
			epsilons.push_back(epsilon);
			fitnesses.push_back(fitness);
			accuracies.push_back(accuracy);
			//                                               p       eps        f             a
			//|.oOOOOOOo.|o.........|,0.5;0.35 0;0.0116,1,0.0951136,0.0128956,0.39724,7,0,2,1,1
		}

		float predAv = 0;

		for (auto& val : predictions)
			predAv += val;
		predAv /= predictions.size();

		float epsilonAv = 0;
		for (auto& val : epsilons)
			epsilonAv += val;
		epsilonAv /= epsilons.size();

		float fitnessAv = 0;
		for (auto& val : fitnesses)
			fitnessAv += val;
		fitnessAv /= fitnesses.size();
		static bool open = true;

		if (ImGui::Begin("Agent", &open, ImGuiWindowFlags_AlwaysAutoResize)) {

			{
				static std::vector<float> FitnessQueue;
				static float FitnessMax = 0;
				static float FitnessMin = -1;
				struct Funcs
				{
					static float Get(void*, int i) { return FitnessQueue[i]; }

				};

				if (FitnessQueue.size() >= 2000) {
					FitnessQueue.erase(FitnessQueue.begin());
				}
				if (fitnessAv > FitnessMax)
					FitnessMax = fitnessAv;
				if (fitnessAv < FitnessMin) {
					FitnessMin = fitnessAv;
				}
				FitnessQueue.push_back(fitnessAv);
				float(*func)(void*, int) = Funcs::Get;
				float FitnessAverage = 0;
				for (auto& val : FitnessQueue) {
					FitnessAverage += val;
				}
				FitnessAverage /= FitnessQueue.size();
				std::string t = std::to_string(FitnessAverage);
				ImGui::PlotLines("Fitness", func, NULL, FitnessQueue.size(), 0, t.c_str(), FitnessMin, FitnessMax, ImVec2(0, 150));
			}

			{
				static std::vector<float> EpsilonQueue;
				static float EpsilonMax = 0;
				static float EpsilonMin = -1;
				struct Funcs
				{
					static float Get(void*, int i) { return EpsilonQueue[i]; }

				};

				if (EpsilonQueue.size() >= 2000) {
					EpsilonQueue.erase(EpsilonQueue.begin());
				}
				if (epsilonAv > EpsilonMax)
					EpsilonMax = epsilonAv;
				if (epsilonAv < EpsilonMin) {
					EpsilonMin = epsilonAv;
				}
				EpsilonQueue.push_back(epsilonAv);
				float(*func)(void*, int) = Funcs::Get;
				float EpsilonAverage = 0;
				for (auto& val : EpsilonQueue) {
					EpsilonAverage += val;
				}
				EpsilonAverage /= EpsilonQueue.size();
				std::string t = std::to_string(EpsilonAverage);
				ImGui::PlotLines("Epsilon", func, NULL, EpsilonQueue.size(), 0, t.c_str(), EpsilonMin, EpsilonMax, ImVec2(0, 150));
			}
			ImGui::End();
		}
	}
}