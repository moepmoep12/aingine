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
		//// In order for the editor to display the scripts name correctly
		//SetName(typeid(*this).name());


		//Constants.n = 100;
		//Constants.beta = 0.1;
		//Constants.dontCareProbability = 0.4;
		//Constants.gamma = 0.6;
		//Constants.thetaGA = 50;
		//Constants.useMAM = true;
		//Constants.epsilonZero = 10;
		//Constants.minValue = -1.1;
		//Constants.maxValue = 1.1;
		//Constants.exploreProbability = 0.5;
		//Constants.doCoveringRandomRangeTruncation = true;
		//TranslationRate = 15;
		//static std::unordered_set<int> options = std::unordered_set<int>{ 0,-1,1 };
		//m_xcsr = new xxr::XCSR<>(options, Constants, xxr::UBR);
	}

	AgentPlayer::~AgentPlayer()
	{
		//delete m_xcsr;
	}

	// Start is called when gameplay starts for this script
	void AgentPlayer::OnStart()
	{
		//Pong::FIXED_TIMESTEP = 1.0 / 120.0;
		//Pong::UseFixedTimeStep(true);
		//Player::OnStart();

		//m_ball = AIngine::World::GetGameObject("Ball")->GetComponent<Ball>();
	}

	// End is called when gameplay ends for this script
	void AgentPlayer::OnEnd()
	{
		//Player::OnEnd();
	}

	// Update is called once per frame

	//static const int tickRate = 0;

	void AgentPlayer::Update(float delta)
	{
		//static float minY = AIngine::World::GetBounds().z + GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;
		//static float maxY = AIngine::World::GetBounds().w - GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;

		//Player::Update(delta);

		//currentTick++;
		//if (currentTick >= tickRate) {
		//	currentTick = 0;

		//	if (m_HasBall)
		//		Player::StartBall();

		//	int action = explore ? m_xcsr->explore(situation()) : m_xcsr->exploit(situation(), true);
		//	lastActionTaken = action;

		//	//if ((action == 1 && GetOwner()->GetWorldPosition().y == maxY)
		//	//	|| (action == -1 && GetOwner()->GetWorldPosition().y == minY)) {
		//	//	EndExperiment(0);
		//	//	return;
		//	//}

		//	Move(action);

		//	if (scored)
		//	{
		//		if (goalie == this->Role) {
		//			EndExperiment(1000);
		//		}
		//		else {
		//			EndExperiment(0);
		//		}
		//		scored = false;
		//		return;
		//	}

		//	else if (m_rigidBody->GetContact() && m_rigidBody->GetContact()->Other->GetOwner()->GetComponent<Ball>()) {
		//		Pong::ScorePlayerTwo++;
		//		EndExperiment(1000);
		//	}
		//	else {
		//		static auto rect = AIngine::World::GetWorldRect();

		//		glm::vec2 worldPos = GetOwner()->GetWorldPosition();
		//		double collisionPoint = ComputeBallCollisionPoint();
		//		double distanceToCollisionPoint = -1;
		//		float reward = 0;
		//		if (collisionPoint < 0 || collisionPoint > rect.height) {
		//			if (action == 0) reward = 10; // out of reach
		//		}
		//		else {
		//			if (std::abs(worldPos.y - collisionPoint) > 0.2) {
		//				if (collisionPoint > worldPos.y) {
		//					if (action == 1) reward = 10;
		//				}
		//				else
		//					if (action == -1) reward = 10;
		//			}
		//			else {
		//				reward = 10; // right position
		//			}
		//		}
		//		m_xcsr->reward(reward, false);
		//	}

		//	if (!explore) {
		//		if (Pong::ScorePlayerOne + Pong::ScorePlayerTwo >= 500) {
		//			DEBUG_INFO("Performace: {0} : {1} | {2}", Pong::ScorePlayerOne, Pong::ScorePlayerTwo, (float)Pong::ScorePlayerTwo / (float)Pong::ScorePlayerOne);
		//			Pong::ScorePlayerOne = 0;
		//			Pong::ScorePlayerTwo = 0;
		//		}
		//	}
		//}
	}

	// Callback for events
	void AgentPlayer::OnEventData(AIngine::Events::EventData & e)
	{
		//// key pressed
		//if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent::KeyPressedEventData)) {
		//	AIngine::Events::KeyPressedEvent::KeyPressedEventData pressedEvent = dynamic_cast<AIngine::Events::KeyPressedEvent::KeyPressedEventData&>(e);

		//	if (pressedEvent.GetKeyCode() == AIngine::KeyCode::R) {
		//		Pong::Get().bRender = !Pong::Get().bRender;
		//	}
		//}
	}

	void AgentPlayer::OnGUI()
	{
	//	static std::vector<double> FitnessValues;
	//	static std::vector<double> AccuracyValues;
	//	static std::vector<double> EpsilonValues;
	//	static double accuracyAverage = 0;
	//	static double accuracyMax = 0;
	//	static double fitnessAverage = 0;
	//	static double epsilonAverage = 0;
	//	static double epsilonMax = 0;
	//	static double predictionAverage = 0;
	//	static bool open = true;

	//	guiTick++;

	//	bool update = guiTick >= guiTicks;

	//	if (!ImGui::Begin("Agent", &open, ImGuiWindowFlags_AlwaysAutoResize))
	//	{
	//		ImGui::End();
	//		return;
	//	}

	//	auto xcsrCS = dynamic_cast<const xxr::xcsr_impl::ubr::Experiment<double, int>*>(&m_xcsr->GetExperiment());
	//	if (xcsrCS) {
	//		auto population = xcsrCS->GetPopulation();
	//		if (update) {
	//			guiTick = 0;
	//			// reset values
	//			accuracyAverage = 0;
	//			fitnessAverage = 0;
	//			epsilonAverage = 0;
	//			predictionAverage = 0;
	//			FitnessValues.clear();
	//			AccuracyValues.clear();

	//			for (auto cl : population) {
	//				accuracyAverage += cl->accuracy();
	//				fitnessAverage += cl->fitness;
	//				epsilonAverage += cl->epsilon;
	//				predictionAverage += cl->prediction;
	//				FitnessValues.push_back(cl->fitness);
	//				AccuracyValues.push_back(cl->accuracy());
	//				EpsilonValues.push_back(cl->epsilon);

	//				if (cl->epsilon > epsilonMax)
	//					epsilonMax = cl->epsilon;

	//				if (cl->accuracy() > accuracyMax)
	//					accuracyMax = cl->accuracy();
	//			}

	//			accuracyAverage /= population.size();
	//			fitnessAverage /= population.size();
	//			epsilonAverage /= population.size();
	//			predictionAverage /= population.size();

	//			std::sort(FitnessValues.begin(), FitnessValues.end());
	//			std::sort(AccuracyValues.begin(), AccuracyValues.end());
	//			std::sort(EpsilonValues.begin(), EpsilonValues.end());
	//		}


	//		// Draw FitnessAverage Graph
	//		{
	//			static std::vector<float> FitnessQueue;
	//			static float FitnessMax = 0;
	//			static float FitnessMin = 10;
	//			static const int maxElements = 2000;
	//			struct Funcs
	//			{
	//				static float Get(void*, int i) { return FitnessQueue[i]; }
	//			};
	//			float(*func)(void*, int) = Funcs::Get;

	//			if (FitnessQueue.size() >= maxElements) {
	//				FitnessQueue.erase(FitnessQueue.begin());
	//			}
	//			if (update)
	//			{
	//				FitnessQueue.push_back(fitnessAverage);
	//				if (fitnessAverage > FitnessMax)
	//					FitnessMax = fitnessAverage;
	//				if (fitnessAverage < FitnessMin) {
	//					FitnessMin = fitnessAverage;
	//				}
	//			}
	//			std::string t = std::to_string(fitnessAverage);
	//			ImGui::PlotLines("Fitness Average", func, NULL, FitnessQueue.size(), 0, t.c_str(), FitnessMin, FitnessMax, ImVec2(0, 150));
	//		}

	//		// Draw FitnessValues Histogramm
	//		{
	//			ImGui::SameLine();
	//			struct Funcs
	//			{
	//				static float Get(void*, int i) { return FitnessValues[i]; }
	//			};
	//			float(*func)(void*, int) = Funcs::Get;

	//			ImGui::PlotHistogram("Fitness Values", func, NULL, FitnessValues.size(), 0, NULL, 0, 1, ImVec2(0, 150));

	//		}

	//		ImGui::Separator();

	//		// Draw Epsilon-Average Graph
	//		{
	//			static std::vector<float> EpsilonQueue;
	//			static float EpsilonMax = 0;
	//			static float EpsilonMin = 10;
	//			static const int maxElements = 2000;
	//			struct Funcs
	//			{
	//				static float Get(void*, int i) { return EpsilonQueue[i]; }
	//			};
	//			float(*func)(void*, int) = Funcs::Get;

	//			if (EpsilonQueue.size() >= maxElements) {
	//				EpsilonQueue.erase(EpsilonQueue.begin());
	//			}

	//			if (update)
	//			{
	//				EpsilonQueue.push_back(epsilonAverage);
	//				if (epsilonAverage > EpsilonMax)
	//					EpsilonMax = epsilonAverage;
	//				if (epsilonAverage < EpsilonMin) {
	//					EpsilonMin = epsilonAverage;
	//				}
	//			}

	//			std::string t = std::to_string(epsilonAverage);
	//			ImGui::PlotLines("Epsilon Average", func, NULL, EpsilonQueue.size(), 0, t.c_str(), EpsilonMin, EpsilonMax, ImVec2(0, 150));
	//		}

	//		// Draw EpsilonValues Histogramm
	//		{
	//			ImGui::SameLine();
	//			struct Funcs
	//			{
	//				static float Get(void*, int i) { return EpsilonValues[i]; }
	//			};
	//			float(*func)(void*, int) = Funcs::Get;

	//			ImGui::PlotHistogram("Epsilon Values", func, NULL, EpsilonValues.size(), 0, NULL, 0, epsilonMax, ImVec2(0, 150));

	//		}

	//		ImGui::Separator();

	//		// Draw AccuracyAverage Graph
	//		{
	//			static std::vector<float> AccuracyQueue;
	//			static float AccuracyMax = 0;
	//			static float AccuracyMin = 10;
	//			static const int maxElements = 2000;
	//			struct Funcs
	//			{
	//				static float Get(void*, int i) { return AccuracyQueue[i]; }
	//			};
	//			float(*func)(void*, int) = Funcs::Get;

	//			if (AccuracyQueue.size() >= maxElements) {
	//				AccuracyQueue.erase(AccuracyQueue.begin());
	//			}

	//			if (update)
	//			{
	//				AccuracyQueue.push_back(accuracyAverage);
	//				if (accuracyAverage > AccuracyMax)
	//					AccuracyMax = accuracyAverage;
	//				if (accuracyAverage < AccuracyMin) {
	//					AccuracyMin = accuracyAverage;
	//				}
	//			}

	//			std::string t = std::to_string(accuracyAverage);
	//			ImGui::PlotLines("Accuracy Average", func, NULL, AccuracyQueue.size(), 0, t.c_str(), AccuracyMin, AccuracyMax, ImVec2(0, 150));
	//		}


	//		// Draw FitnessValues Histogramm
	//		{
	//			ImGui::SameLine();
	//			struct Funcs
	//			{
	//				static float Get(void*, int i) { return AccuracyValues[i]; }
	//			};
	//			float(*func)(void*, int) = Funcs::Get;

	//			ImGui::PlotHistogram("Accuracy Values", func, NULL, AccuracyValues.size(), 0, NULL, 0, accuracyMax, ImVec2(0, 150));

	//		}

	//		ImGui::NewLine();

	//		std::stringstream ss;
	//		ss << "Population Size : " << population.size();
	//		ImGui::Text(ss.str().c_str());

	//		ss.str(std::string());
	//		ss << "Numerosity Sum : " << m_xcsr->numerositySum();
	//		ImGui::Text(ss.str().c_str());

	//		ImGui::End();
	//	}
	//}

	//void AgentPlayer::OnWidget()
	//{
	//	std::stringstream label;

	//	ImGui::Checkbox("Give Enemy Ball?", &giveEnemyBall);

	//	label << "Explore?##" << static_cast<const void*>(this);
	//	if (ImGui::Checkbox(label.str().c_str(), &explore)) {
	//		Pong::ScorePlayerOne = 0;
	//		Pong::ScorePlayerTwo = 0;
	//	}

	//	label.str(std::string());
	//	label << "Population Size : " << m_xcsr->populationSize();
	//	ImGui::Text(label.str().c_str());

	//	label.str(std::string());
	//	label << "Save Population##" << static_cast<const void*>(this);
	//	if (ImGui::Button(label.str().c_str())) {
	//		static const char* filterList = "txt,json";
	//		std::string path;
	//		AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::SaveFile(filterList, &path, "");

	//		if (result == AIngine::Util::Filesystem::Result::OKAY)
	//		{
	//			std::ofstream file;
	//			file.open(path);
	//			if (file.fail()) return;
	//			std::ostringstream os;
	//			m_xcsr->dumpPopulation(os);
	//			std::string content = os.str();
	//			file << content;
	//			file.close();
	//		}
	//	}

	//	label.str(std::string());
	//	label << "Load Population##" << static_cast<const void*>(this);

	//	if (ImGui::Button(label.str().c_str())) {
	//		static const char* filterList = "txt,json";
	//		std::string path;
	//		AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::OpenFile(filterList, &path, "");

	//		if (result == AIngine::Util::Filesystem::Result::OKAY)
	//		{
	//			m_xcsr->loadPopulationCSV(path, true);
	//		}
	//	}
	//	label.str(std::string());
	//	label << "Condensation Mode##" << static_cast<const void*>(this);

	//	if (ImGui::Button(label.str().c_str())) {
	//		m_xcsr->switchToCondensationMode();
	//	}

	//	static bool showPopulation = false;
	//	if (ImGui::Button("ShowPopulation")) {
	//		showPopulation = !showPopulation;
	//	}
	//	if (showPopulation) ShowPopulation(showPopulation);

	//	auto xcsrCS = dynamic_cast<xxr::xcsr_impl::ubr::Experiment<double, int>*>(&m_xcsr->GetExperiment());
	//	if (xcsrCS) {
	//		CreateConstantsWidget((xxr::XCSRConstants&)(xcsrCS->GetConstants()));
	//	}

	}

	void AgentPlayer::OnScored(PlayerRole role)
	{
		//goalie = role;
		//scored = true;
	}

	void AgentPlayer::CreateConstantsWidget(xxr::XCSRConstants & constants)
	{

	//	int popSize = (int)constants.n;
	//	if (ImGui::DragInt("Population Size", &popSize)) {
	//		constants.n = popSize;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("  The maximum size of the population \n"
	//			"(the sum of the classifier numerosities in micro-classifiers) \n"
	//			" Recommended: large enough");
	//		ImGui::EndTooltip();
	//	}


	//	float alpha = constants.alpha;
	//	if (ImGui::DragFloat("Alpha", &alpha)) {
	//		constants.alpha = alpha;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("The fall of rate in the fitness evaluation \n"
	//			"Recommended: 0.1");
	//		ImGui::EndTooltip();
	//	}


	//	float beta = constants.beta;
	//	if (ImGui::DragFloat("Beta", &beta)) {
	//		constants.beta = beta;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("The learning rate for updating fitness, prediction, prediction error, and\n"
	//			"action set size estimate in XCS's classifiers\n"
	//			"Recommended: 0.1-0.2");
	//		ImGui::EndTooltip();
	//	}


	//	float chi = constants.chi;
	//	if (ImGui::DragFloat("chi", &chi)) {
	//		constants.chi = chi;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("The probability of applying crossover\n"
	//			"Recommended: 0.5-1.0");
	//		ImGui::EndTooltip();
	//	}


	//	float coveringMaxSpread = constants.coveringMaxSpread;
	//	if (ImGui::DragFloat("coveringMaxSpread", &coveringMaxSpread)) {
	//		constants.coveringMaxSpread = coveringMaxSpread;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text(" The maximum value of a spread in the covering operator");
	//		ImGui::EndTooltip();
	//	}

	//	float delta = constants.delta;
	//	if (ImGui::DragFloat("delta", &delta)) {
	//		constants.delta = delta;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("The fraction of the mean fitness of the population below which the fitness\n"
	//			"of a classifier may be considered in its vote for deletion\n"
	//			"Recommended: 0.1");
	//		ImGui::EndTooltip();
	//	}

	//	ImGui::Checkbox("doActionMutation", &constants.doActionMutation);
	//	ImGui::Checkbox("doActionSetSubsumption", &constants.doActionSetSubsumption);
	//	ImGui::Checkbox("doGASubsumption", &constants.doGASubsumption);


	//	float dontCareProbability = constants.dontCareProbability;
	//	if (ImGui::DragFloat("dontCareProbability", &dontCareProbability)) {
	//		constants.dontCareProbability = dontCareProbability;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("The probability of using a don't care symbol in an allele when covering\n"
	//			"Recommended: 0.33");
	//		ImGui::EndTooltip();
	//	}


	//	float epsilonZero = constants.epsilonZero;
	//	if (ImGui::DragFloat("epsilonZero", &epsilonZero)) {
	//		constants.epsilonZero = epsilonZero;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text(" The error threshold under which the accuracy of a classifier is set to one\n"
	//			"Recommended: 1% of the maximum reward");
	//		ImGui::EndTooltip();
	//	}


	//	float exploreProbability = constants.exploreProbability;
	//	if (ImGui::DragFloat("exploreProbability", &exploreProbability)) {
	//		constants.exploreProbability = exploreProbability;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("The probability during action selection of choosing the action uniform randomly\n"
	//			"Recommended: 0.5 (depends on the type of experiment)");
	//		ImGui::EndTooltip();
	//	}


	//	float gamma = constants.gamma;
	//	if (ImGui::DragFloat("gamma", &gamma)) {
	//		constants.gamma = gamma;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("The discount rate in multi-step problems\n"
	//			"Recommended: 0.71");
	//		ImGui::EndTooltip();
	//	}


	//	float maxValue = constants.maxValue;
	//	if (ImGui::DragFloat("maxValue", &maxValue)) {
	//		constants.maxValue = maxValue;
	//	}


	//	float minValue = constants.minValue;
	//	if (ImGui::DragFloat("minValue", &minValue)) {
	//		constants.minValue = minValue;
	//	}


	//	float mu = constants.mu;
	//	if (ImGui::DragFloat("mu", &mu)) {
	//		constants.mu = mu;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("The probability of mutating one allele and the action\n"
	//			"Recommended: 0.01-0.05");
	//		ImGui::EndTooltip();
	//	}


	//	float mutationMaxChange = constants.mutationMaxChange;
	//	if (ImGui::DragFloat("mutationMaxChange", &mutationMaxChange)) {
	//		constants.mutationMaxChange = mutationMaxChange;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text(" The maximum change of a spread value or a center value in the mutation");
	//		ImGui::EndTooltip();
	//	}


	//	float nu = constants.nu;
	//	if (ImGui::DragFloat("nu", &nu)) {
	//		constants.nu = nu;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("The exponent in the power function for the fitness evaluation\n"
	//			"Recommended: 5");
	//		ImGui::EndTooltip();
	//	}


	//	float subsumptionTolerance = constants.subsumptionTolerance;
	//	if (ImGui::DragFloat("subsumptionTolerance", &subsumptionTolerance)) {
	//		constants.subsumptionTolerance = subsumptionTolerance;
	//	}


	//	float tau = constants.tau;
	//	if (ImGui::DragFloat("tau", &tau)) {
	//		constants.tau = tau;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("The tournament size for selection [Butz et al., 2003]\n"
	//			"(set 0 to use the roulette-wheel selection)");
	//		ImGui::EndTooltip();
	//	}

	//	int thetaDel = constants.thetaDel;
	//	if (ImGui::DragInt("thetaDel", &thetaDel)) {
	//		constants.thetaDel = thetaDel;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("The experience threshold over which the fitness of a classifier may be\n"
	//			"considered in its deletion probability\n"
	//			"Recommended: 20");
	//		ImGui::EndTooltip();
	//	}


	//	int thetaGA = constants.thetaGA;
	//	if (ImGui::DragInt("thetaGA", &thetaGA)) {
	//		constants.thetaGA = thetaGA;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("The threshold for the GA application in an action set\n"
	//			"Recommended: 25-50");
	//		ImGui::EndTooltip();
	//	}


	//	int thetaMna = constants.thetaMna;
	//	if (ImGui::DragInt("thetaMna", &thetaMna)) {
	//		constants.thetaMna = thetaMna;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("        //   The minimal number of actions that must be present in a match set [M],\n"
	//			"or else covering will occur\n"
	//			"Recommended: the number of available actions\n"
	//			"(or use 0 to set automatically)");
	//		ImGui::EndTooltip();
	//	}


	//	int thetaSub = constants.thetaSub;
	//	if (ImGui::DragInt("thetaSub", &thetaSub)) {
	//		constants.thetaSub = thetaSub;
	//	}
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::BeginTooltip();
	//		ImGui::Text("The experience of a classifier required to be a subsumer\n"
	//			"Recommended: 20");
	//		ImGui::EndTooltip();
	//	}


	//	ImGui::Checkbox("Use MAM", &constants.useMAM);
	//}

	//std::vector<double> AgentPlayer::situation()
	//{
	//	static auto rect = AIngine::World::GetWorldRect();
	//	Player* other = Role == PlayerRole::One ? m_ball->PlayerTwo : m_ball->PlayerOne;

	//	glm::vec2 worldPos = GetOwner()->GetWorldPosition();
	//	glm::vec2 ballPos = m_ball->GetOwner()->GetWorldPosition();
	//	double collisionPoint = ComputeBallCollisionPoint();
	//	double distanceToCollisionPoint = -1;

	//	if (collisionPoint < 0 || collisionPoint > rect.height)
	//		collisionPoint = -1;
	//	else {
	//		Graphics::Point(glm::vec2(worldPos.x, collisionPoint), 10, glm::vec4(0, 0, 0, 1));
	//		distanceToCollisionPoint = (worldPos.y - collisionPoint) / rect.height;
	//		collisionPoint /= rect.height;
	//	}

	//	std::vector<double> result =
	//	{
	//		worldPos.y / rect.height, // relative height
	//		glm::distance(worldPos, ballPos) / rect.width, // relative distance to ball
	//		std::clamp(m_BallBody->GetVelocity().x / 10.0,-1.0,1.0), // relative ball velocity.x
	//		std::clamp(m_BallBody->GetVelocity().y / 10.0,-1.0,1.0), // relative ball velocity.y
	//		ballPos.x / rect.width, // relative ballpos.x
	//		ballPos.y / rect.height, // relative ballpos.y
	//		(double)lastActionTaken, // last action taken
	//		(5.625 - ballPos.y) / rect.height, // relative ball distance to bottom edge
	//		std::clamp(collisionPoint,-1.0,1.0),
	//		std::clamp(distanceToCollisionPoint,-1.0,1.0),
	//		//other->GetOwner()->GetWorldPosition().y / rect.height
	//	};

	//	return result;
	}
	void AgentPlayer::EndExperiment(float reward)
	{
		//Player* other = Role == PlayerRole::One ? m_ball->PlayerTwo : m_ball->PlayerOne;
		//if (giveEnemyBall) other->ReceiveBall();
		//m_xcsr->reward(reward, true);
	}

	//double AgentPlayer::ComputeBallCollisionPoint()
	//{
	//	//glm::vec2 ballPos = m_ball->GetOwner()->GetWorldPosition();
	//	//glm::vec2 ballVel = m_BallBody->GetVelocity();
	//	//double axis = GetOwner()->GetWorldPosition().x;

	//	///* y = v.y/v.x * x + (p.y - v.y/v.x * p.x)  */
	//	//if (ballVel.length() == 0) return -10;
	//	//return (ballVel.y / ballVel.x) * axis + (ballPos.y - (ballVel.y / ballVel.x) * ballPos.x);
	//}

	void AgentPlayer::ShowPopulation(bool& show)
	{
		auto xcsrCS = dynamic_cast<xxr::xcsr_impl::ubr::Experiment<double, int>*>(&m_xcsr->GetExperiment());
		const int max = 25;
		if (xcsrCS) {
			auto population = xcsrCS->GetPopulation();

			if (!ImGui::Begin("Population", &show, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::End();
				return;
			}

			std::vector < xxr::xcs_impl::ClassifierPtrSet<xxr::xcsr_impl::StoredClassifier<xxr::xcs_impl::Classifier< xxr::xcsr_impl::ConditionActionPair<xxr::xcsr_impl::Condition<xxr::xcsr_impl::ubr::Symbol<double>>, int>>, xxr::xcsr_impl::Constants>>::ClassifierPtr> classifiers;
			classifiers.reserve(population.size());
			for (auto& cl : population) {
				classifiers.push_back(cl);
			}

			std::sort(classifiers.begin(), classifiers.end(), [](auto x, auto y) {
				return x->fitness > y->fitness;
			});

			int i = 0;
			for (auto& cl : classifiers) {
				i++;
				if (i > max) break;
				std::stringstream label;
				label << "Numerosity: " << cl->numerosity << " | Fitness: " << cl->fitness << " | Accuracy: " << cl->accuracy() << " | Error: " << cl->epsilon << "\n";
				label << "Exp: " << cl->experience << " | Prediction: " << cl->prediction;
				ImGui::Text(label.str().c_str());
				ImGui::Separator();
			}

			ImGui::End();
		}
	}
}