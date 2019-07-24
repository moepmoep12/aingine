#include "Editor/Widgets/AgentComponentWidget.h"
#include "Editor/Widgets/Common.h"
#include "AIngine/XCSAgents.h"
#include "Util/FileSystem.h"

#include <iostream>
#include <unordered_map>

namespace AIngine::Editor::Widget::Component {

	AIngine::XCSAgentSupervisor* dropTarget = nullptr;
	static bool added = false;

	AgentSupervisorComponentWidget::AgentSupervisorComponentWidget()
	{
		OnSelectionChangedHandler = AIngine::Editor::Editor::SelectionChangedEventHandler(std::bind(&AgentSupervisorComponentWidget::OnSelectionChanged, this, std::placeholders::_1));
	}

	void AgentSupervisorComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			AIngine::GameObject* obj = m_activeGameObjects[0];
			using SuperVisor = AIngine::XCSAgentSupervisor;
			SuperVisor* supervisor = obj->GetComponent<SuperVisor>();
			if (supervisor)
			{
				if (added) {
					added = false;
					AIngine::Editor::Editor::OnSelectionChangedEvent -= OnSelectionChangedHandler;
				}

				std::stringstream label;

				// TITLE
				if(!DisplayTitle(supervisor, "AgentSupervisor")) return;

				ImGui::NewLine();

				/// General Settings
				// Observation Size
				ImGui::DragInt("Observations Size", &supervisor->ObservationsSize);
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("How many features the agent observes \n"
						"(Will be set automatically)");
					ImGui::EndTooltip();
				}
				// StepRate
				ImGui::DragInt("Step Rate", &supervisor->StepRate);
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("The frequency at which the agent \n"
						"will observe & act");
					ImGui::EndTooltip();
				}
				//MaxSteps
				ImGui::DragInt("Max Steps", &supervisor->MaxSteps);
				//Exploration
				ImGui::Checkbox("Exploration", &supervisor->Exploration);
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("In Exploration actions are chosen randomly according \n"
						"to the probability, else greedy ");
					ImGui::EndTooltip();
				}

				// CondensationMode
				bool isInCondensationmode = supervisor->IsInCondensationMode();
				if (ImGui::Checkbox("Condensation Mode", &isInCondensationmode)) {
					supervisor->SwitchCondensationMode(isInCondensationmode);
				}
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("In Condensation Mode Crossover (chi), \n"
						"Mutation (mu) & GA Subsumption is turned off");
					ImGui::EndTooltip();
				}

				// Recording
				static std::map<SuperVisor*, bool> recordMap;
				bool recording = false;
				label.str(std::string());
				if (recordMap.find(supervisor) == recordMap.end()) {
					recordMap[supervisor] = false;
				}
				else {
					recording = recordMap[supervisor];
				}
				std::string btnText = !recording ? "Start Recording##" : "Stop Recording##";
				label << btnText << static_cast<const void*>(supervisor);
				if (ImGui::Button(label.str().c_str()))
				{
					supervisor->SetRecording(!recording);
					recordMap[supervisor] = !recording;
				}
				ImGui::SameLine();
				label.str(std::string());
				label << "Record Frequency##" << static_cast<const void*>(supervisor);
				int recordFrequency = supervisor->GetRecordInterval();
				if (ImGui::InputInt(label.str().c_str(), &recordFrequency)) {
					supervisor->SetRecordInterval(recordFrequency);
				}

				// Show SnapShots
				label.str(std::string());
				static std::map<SuperVisor*, bool> showMap;
				bool showing = false;
				if (showMap.find(supervisor) == showMap.end()) {
					showMap[supervisor] = false;
				}
				else {
					showing = showMap[supervisor];
				}
				btnText = !showing ? "Show Snapshots##" : "Hide Snapshots##";
				label << btnText << static_cast<const void*>(supervisor);
				if (ImGui::Button(label.str().c_str()))
				{
					showMap[supervisor] = !showing;
				}
				if (showMap[supervisor]) DrawPopulationSnapShots(*supervisor);

				// Representation
				static const xxr::XCSRRepr choosableRepresentations[] = { xxr::XCSRRepr::CSR, xxr::XCSRRepr::OBR, xxr::XCSRRepr::UBR };
				static const char* representatationsNames[] = { "CSR", "OBR", "UBR" };
				int currentRepr = (int)supervisor->GetRepresentation();
				label.str(std::string());
				label << "Representatation##" << static_cast<const void*>(supervisor);
				if (ImGui::BeginCombo(label.str().c_str(), representatationsNames[currentRepr])) {
					for (int i = 0; i < IM_ARRAYSIZE(representatationsNames); i++) {
						bool isSelected = (int)choosableRepresentations[i] == currentRepr;
						if (ImGui::Selectable(representatationsNames[i], isSelected)) {
							currentRepr = (int)choosableRepresentations[i];
							supervisor->ChangeRepresentation((xxr::XCSRRepr)currentRepr);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				label.str(std::string());
				label << "Population Size : " << supervisor->m_xcsr->populationSize();
				ImGui::Text(label.str().c_str());

				// Show Population
				static bool show = false;
				static int showAmount = 25;
				if (ImGui::Button("ShowPopulation")) {
					show = !show;
				}
				if (show) ShowPopulation(*supervisor, showAmount);
				ImGui::SameLine();
				ImGui::InputInt("Show Amount", &showAmount);

				// Save Population
				if (ImGui::Button("Save Population")) {
					static const char* filterList = "txt,json";
					std::string path;
					AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::SaveFile(filterList, &path, "");

					if (result == AIngine::Util::Filesystem::Result::OKAY)
					{
						std::ofstream file;
						file.open(path);
						if (file.fail()) return;
						std::ostringstream os;
						supervisor->m_xcsr->dumpPopulation(os);
						supervisor->PopulationPath = path;
						std::string content = os.str();
						file << content;
						file.close();
					}
				}

				// Load Population
				if (ImGui::Button("Load Population")) {
					static const char* filterList = "txt,json";
					std::string path;
					AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::OpenFile(filterList, &path, "");

					if (result == AIngine::Util::Filesystem::Result::OKAY)
					{
						supervisor->m_xcsr->loadPopulationCSV(path, false);
						supervisor->PopulationPath = path;
					}
				}

				// Possible Actions
				{
					ImGui::BeginChild("Possible Actions", ImVec2(0, 150), true, ImGuiWindowFlags_AlwaysAutoResize);

					static int selected = 0;
					bool deleteAction = false;

					for (auto action : supervisor->PossibleActions) {
						bool isSelected = selected == action;
						ImGui::Selectable(std::to_string(action).c_str(), &isSelected);
						if (ImGui::IsItemClicked(0))
							selected = action;

						if (ImGui::BeginPopupContextItem("Remove Action")) {
							if (ImGui::Selectable("Remove##remove action")) {
								ImGui::EndPopup();
								deleteAction = true;
								break;
							}
							ImGui::EndPopup();
						}
					}

					static int actionToAdd = 0;
					ImGui::InputInt("New Action", &actionToAdd);
					ImGui::SameLine();
					if (ImGui::Button("Add Action##xcs action")) {
						if (std::find(supervisor->PossibleActions.begin(), supervisor->PossibleActions.end(), actionToAdd) == supervisor->PossibleActions.end()) {
							supervisor->PossibleActions.insert(actionToAdd);
						}
					}

					if (deleteAction) {
						supervisor->PossibleActions.erase(selected);
					}

					ImGui::EndChild();
				}

				// Agents
				{
					ImGui::BeginChild("Agents", ImVec2(0, 150), true, ImGuiWindowFlags_AlwaysAutoResize);

					static Agent* selectedAgent = nullptr;
					Agent* toDelete = nullptr;
					for (auto agent : supervisor->GetAgents()) {
						bool isSelected = selectedAgent == agent;
						ImGui::Selectable(agent->Name().c_str(), &isSelected);
						if (ImGui::IsItemClicked(0))
							selectedAgent = agent;

						std::stringstream label;
						label << "Remove " << agent->Name() << "##" << static_cast<const void*>(agent);
						if (ImGui::BeginPopupContextItem("Remove Agents")) {
							if (ImGui::Selectable(label.str().c_str())) {
								ImGui::EndPopup();
								toDelete = agent;
								break;
							}
							ImGui::EndPopup();
						}
					}
					if (toDelete)supervisor->RemoveAgent(toDelete);

					if (ImGui::Button("Add Agent")) {
						AIngine::Editor::Editor::OnSelectionChangedEvent += OnSelectionChangedHandler;
						dropTarget = supervisor;
					}

					ImGui::EndChild();
				}

				ImGui::NewLine();

				// Constants
				{
					auto& constants = supervisor->GetConstants();

					int popSize = (int)constants.n;
					if (ImGui::DragInt("Population Size", &popSize)) {
						constants.n = popSize;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("  The maximum size of the population \n"
							"(the sum of the classifier numerosities in micro-classifiers) \n"
							" Recommended: large enough");
						ImGui::EndTooltip();
					}


					float alpha = constants.alpha;
					if (ImGui::DragFloat("Alpha", &alpha)) {
						constants.alpha = alpha;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("The fall of rate in the fitness evaluation \n"
							"Recommended: 0.1");
						ImGui::EndTooltip();
					}


					float beta = constants.beta;
					if (ImGui::DragFloat("Beta", &beta)) {
						constants.beta = beta;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("The learning rate for updating fitness, prediction, prediction error, and\n"
							"action set size estimate in XCS's classifiers\n"
							"Recommended: 0.1-0.2");
						ImGui::EndTooltip();
					}


					float chi = constants.chi;
					if (ImGui::DragFloat("chi", &chi)) {
						constants.chi = chi;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("The probability of applying crossover\n"
							"Recommended: 0.5-1.0");
						ImGui::EndTooltip();
					}


					float coveringMaxSpread = constants.coveringMaxSpread;
					if (ImGui::DragFloat("coveringMaxSpread", &coveringMaxSpread)) {
						constants.coveringMaxSpread = coveringMaxSpread;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(" The maximum value of a spread in the covering operator");
						ImGui::EndTooltip();
					}

					float delta = constants.delta;
					if (ImGui::DragFloat("delta", &delta)) {
						constants.delta = delta;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("The fraction of the mean fitness of the population below which the fitness\n"
							"of a classifier may be considered in its vote for deletion\n"
							"Recommended: 0.1");
						ImGui::EndTooltip();
					}

					float dontCareProbability = constants.dontCareProbability;
					if (ImGui::DragFloat("dontCareProbability", &dontCareProbability)) {
						constants.dontCareProbability = dontCareProbability;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("The probability of using a don't care symbol in an allele when covering\n"
							"Recommended: 0.33");
						ImGui::EndTooltip();
					}


					float epsilonZero = constants.epsilonZero;
					if (ImGui::DragFloat("epsilonZero", &epsilonZero)) {
						constants.epsilonZero = epsilonZero;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(" The error threshold under which the accuracy of a classifier is set to one\n"
							"Recommended: 1% of the maximum reward");
						ImGui::EndTooltip();
					}


					float exploreProbability = constants.exploreProbability;
					if (ImGui::DragFloat("exploreProbability", &exploreProbability)) {
						constants.exploreProbability = exploreProbability;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("The probability during action selection of choosing the action uniform randomly\n"
							"Recommended: 0.5 (depends on the type of experiment)");
						ImGui::EndTooltip();
					}


					float gamma = constants.gamma;
					if (ImGui::DragFloat("gamma", &gamma)) {
						constants.gamma = gamma;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("The discount rate in multi-step problems\n"
							"Recommended: 0.71");
						ImGui::EndTooltip();
					}


					float maxValue = constants.maxValue;
					if (ImGui::DragFloat("maxValue", &maxValue)) {
						constants.maxValue = maxValue;
					}


					float minValue = constants.minValue;
					if (ImGui::DragFloat("minValue", &minValue)) {
						constants.minValue = minValue;
					}


					float mu = constants.mu;
					if (ImGui::DragFloat("mu", &mu)) {
						constants.mu = mu;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("The probability of mutating one allele and the action\n"
							"Recommended: 0.01-0.05");
						ImGui::EndTooltip();
					}


					float mutationMaxChange = constants.mutationMaxChange;
					if (ImGui::DragFloat("mutationMaxChange", &mutationMaxChange)) {
						constants.mutationMaxChange = mutationMaxChange;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text(" The maximum change of a spread value or a center value in the mutation");
						ImGui::EndTooltip();
					}


					float nu = constants.nu;
					if (ImGui::DragFloat("nu", &nu)) {
						constants.nu = nu;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("The exponent in the power function for the fitness evaluation\n"
							"Recommended: 5");
						ImGui::EndTooltip();
					}


					float subsumptionTolerance = constants.subsumptionTolerance;
					if (ImGui::DragFloat("subsumptionTolerance", &subsumptionTolerance)) {
						constants.subsumptionTolerance = subsumptionTolerance;
					}


					float tau = constants.tau;
					if (ImGui::DragFloat("tau", &tau)) {
						constants.tau = tau;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("The tournament size for selection [Butz et al., 2003]\n"
							"(set 0 to use the roulette-wheel selection)");
						ImGui::EndTooltip();
					}

					int thetaDel = constants.thetaDel;
					if (ImGui::DragInt("thetaDel", &thetaDel)) {
						constants.thetaDel = thetaDel;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("The experience threshold over which the fitness of a classifier may be\n"
							"considered in its deletion probability\n"
							"Recommended: 20");
						ImGui::EndTooltip();
					}


					int thetaGA = constants.thetaGA;
					if (ImGui::DragInt("thetaGA", &thetaGA)) {
						constants.thetaGA = thetaGA;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("The threshold for the GA application in an action set\n"
							"Recommended: 25-50");
						ImGui::EndTooltip();
					}


					int thetaMna = constants.thetaMna;
					if (ImGui::DragInt("thetaMna", &thetaMna)) {
						constants.thetaMna = thetaMna;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("        //   The minimal number of actions that must be present in a match set [M],\n"
							"or else covering will occur\n"
							"Recommended: the number of available actions\n"
							"(or use 0 to set automatically)");
						ImGui::EndTooltip();
					}


					int thetaSub = constants.thetaSub;
					if (ImGui::DragInt("thetaSub", &thetaSub)) {
						constants.thetaSub = thetaSub;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("The experience of a classifier required to be a subsumer\n"
							"Recommended: 20");
						ImGui::EndTooltip();
					}

					ImGui::Checkbox("doCoveringRandomRangeTruncation", &constants.doCoveringRandomRangeTruncation);
					ImGui::Checkbox("doActionMutation", &constants.doActionMutation);
					ImGui::Checkbox("doActionSetSubsumption", &constants.doActionSetSubsumption);
					ImGui::Checkbox("doGASubsumption", &constants.doGASubsumption);
					ImGui::Checkbox("Use MAM", &constants.useMAM);

				}

				ImGui::NewLine();


				ImGui::Separator();

			}
		}
	}

	void AgentSupervisorComponentWidget::OnSelectionChanged(const std::vector<AIngine::GameObject*>& selectedObjects)
	{
		added = true;
		if (selectedObjects.size() > 0) {
			AIngine::GameObject* obj = selectedObjects[0];
			using Agent = AIngine::Agent;
			Agent* agent = nullptr;
			for (auto& comp : obj->GetComponents()) {
				if (dynamic_cast<Agent*>(comp))
					agent = dynamic_cast<Agent*>(comp);
			}

			if (agent)
			{
				if (dropTarget) {
					dropTarget->AddAgent(agent);
				}
			}
			else
				dropTarget = nullptr;

		}
	}

	void AgentSupervisorComponentWidget::ShowPopulation(AIngine::XCSAgentSupervisor& supervisor, int amount)
	{
		xxr::xcsr_impl::Experiment<double, int>* xcs = supervisor.m_xcsr.get();

		switch (supervisor.GetRepresentation()) {
		case xxr::XCSRRepr::CSR:
			ShowPopulationCSR(supervisor, amount);
			return;

		case xxr::XCSRRepr::UBR:
			ShowPopulationUBR(supervisor, amount);
			return;

		case xxr::XCSRRepr::OBR:
			ShowPopulationOBR(supervisor, amount);
			return;
		}
	}

	void AgentSupervisorComponentWidget::ShowPopulationCSR(AIngine::XCSAgentSupervisor & supervisor, int amount)
	{
		static bool show = true;
		auto xcsrCS = dynamic_cast<xxr::xcsr_impl::csr::Experiment<double, int>*>(&supervisor.m_xcsr.get()->GetExperiment());
		if (xcsrCS) {
			auto population = xcsrCS->GetPopulation();

			if (!ImGui::Begin("Population", &show, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::End();
				return;
			}

			std::vector < xxr::xcs_impl::ClassifierPtrSet<xxr::xcsr_impl::StoredClassifier<xxr::xcs_impl::Classifier< xxr::xcsr_impl::ConditionActionPair<xxr::xcsr_impl::Condition<xxr::xcsr_impl::csr::Symbol<double>>, int>>, xxr::xcsr_impl::Constants>>::ClassifierPtr> classifiers;
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
				if (i > amount) break;
				std::stringstream label;
				label << "Numerosity: " << cl->numerosity << " | Fitness: " << cl->fitness << " | Accuracy: " << cl->accuracy() << " | Error: " << cl->epsilon << "\n";
				label << "Exp: " << cl->experience << " | Prediction: " << cl->prediction;
				ImGui::Text(label.str().c_str());
				ImGui::Separator();
			}

			ImGui::End();
		}
	}

	void AgentSupervisorComponentWidget::ShowPopulationUBR(AIngine::XCSAgentSupervisor & supervisor, int amount)
	{
		static bool show = true;
		auto xcsrCS = dynamic_cast<xxr::xcsr_impl::ubr::Experiment<double, int>*>(&supervisor.m_xcsr.get()->GetExperiment());
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
				if (i > amount) break;
				std::stringstream label;
				label << "Numerosity: " << cl->numerosity << " | Fitness: " << cl->fitness << " | Accuracy: " << cl->accuracy() << " | Error: " << cl->epsilon << "\n";
				label << "Exp: " << cl->experience << " | Prediction: " << cl->prediction;
				ImGui::Text(label.str().c_str());
				ImGui::Separator();
			}

			ImGui::End();
		}
	}

	void AgentSupervisorComponentWidget::ShowPopulationOBR(AIngine::XCSAgentSupervisor & supervisor, int amount)
	{
		static bool show = true;
		auto xcsrCS = dynamic_cast<xxr::xcsr_impl::obr::Experiment<double, int>*>(&supervisor.m_xcsr.get()->GetExperiment());
		if (xcsrCS) {
			auto population = xcsrCS->GetPopulation();

			if (!ImGui::Begin("Population", &show, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::End();
				return;
			}

			std::vector < xxr::xcs_impl::ClassifierPtrSet<xxr::xcsr_impl::StoredClassifier<xxr::xcs_impl::Classifier< xxr::xcsr_impl::ConditionActionPair<xxr::xcsr_impl::Condition<xxr::xcsr_impl::obr::Symbol<double>>, int>>, xxr::xcsr_impl::Constants>>::ClassifierPtr> classifiers;
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
				if (i > amount) break;
				std::stringstream label;
				label << "Numerosity: " << cl->numerosity << " | Fitness: " << cl->fitness << " | Accuracy: " << cl->accuracy() << " | Error: " << cl->epsilon << "\n";
				label << "Exp: " << cl->experience << " | Prediction: " << cl->prediction;
				ImGui::Text(label.str().c_str());
				ImGui::Separator();
			}

			ImGui::End();
		}
	}

	void AgentSupervisorComponentWidget::DrawPopulationSnapShots(AIngine::XCSAgentSupervisor & supervisor)
	{
		static bool open = true;
		if (!ImGui::Begin("Snapshots", &open, ImGuiWindowFlags_NoDocking)) {
			ImGui::End();
			return;
		}
		const int stride = sizeof(AIngine::XCSAgentSupervisor::PopulationSnapshot);
		const int count = supervisor.GetSnapShots().size();

		if (supervisor.GetSnapShots().size() > 0) {

			//ImGui::PlotLines("Numerosity Sum", &supervisor.GetSnapShots()[0].NumerositySum, count, 0, NULL, 0.0f, 2 * supervisor.GetConstants().n, ImVec2(0, 150), stride);
			//ImGui::PlotLines("Macroclassifiers", &supervisor.GetSnapShots()[0].MacroClassifierCount, count, 0, NULL, 0.0f, supervisor.GetConstants().n, ImVec2(0, 150), stride);
			ImGui::PlotLines("Fitness", &supervisor.GetSnapShots()[0].AverageFitness, count, 0, NULL, 0.0f, supervisor.GetMaxFitness(), ImVec2(0, 150), stride);
			ImGui::PlotLines("Accuracy", &supervisor.GetSnapShots()[0].AverageAccuracy, count, 0, NULL, 0.0f, 1.0f, ImVec2(0, 150), stride);
			ImGui::PlotLines("Epsilon", &supervisor.GetSnapShots()[0].AverageEpsilon, count, 0, NULL, 0.0f, supervisor.GetMaxEpsilon(), ImVec2(0, 150), stride);
			ImGui::PlotLines("Prediction", &supervisor.GetSnapShots()[0].AveragePrediction, count, 0, NULL, 0.0f, supervisor.GetMaxPrediction(), ImVec2(0, 150), stride);
			ImGui::PlotLines("ActionSet Size", &supervisor.GetSnapShots()[0].ActionSetSize, count, 0, NULL, 0.0f, supervisor.GetMaxActionSetSize(), ImVec2(0, 150), stride);
		}

		ImGui::End();


		if (!ImGui::Begin("Episodes", &open, ImGuiWindowFlags_NoDocking)) {
			ImGui::End();
			return;
		}

		const int episodeStride = sizeof(AIngine::XCSAgentSupervisor::EpisodeSnapshot);

		if (supervisor.GetEpisodeSnapshots().size() > 0) {
			ImGui::PlotLines("Reward", &supervisor.GetEpisodeSnapshots()[0].RewardSum, supervisor.GetEpisodeSnapshots().size(), 0, NULL, 0.0f, supervisor.GetMaxReward(), ImVec2(0, 150), episodeStride);
			ImGui::PlotLines("EpisodeLength", &supervisor.GetEpisodeSnapshots()[0].DurationInTicks, supervisor.GetEpisodeSnapshots().size(), 0, NULL, 0.0f, supervisor.GetMaxDuration(), ImVec2(0, 150), episodeStride);
		}

		ImGui::End();

	}



	void AgentComponentWidget::OnImGuiRender()
	{
	}
}