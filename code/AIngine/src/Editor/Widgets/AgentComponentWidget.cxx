#include "Editor/Widgets/AgentComponentWidget.h"
#include "Editor/Widgets/Common.h"
#include "AIngine/XCSAgents.h"
#include "Util/FileSystem.h"

#include <iostream>

namespace AIngine::Editor::Widget::Component {

	AIngine::Agent* dragAgent = nullptr;
	AIngine::XCSAgentSupervisor* dropAgent = nullptr;


	void AgentSupervisorComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			AIngine::GameObject* obj = m_activeGameObjects[0];
			using SuperVisor = AIngine::XCSAgentSupervisor;
			SuperVisor* supervisor = obj->GetComponent<SuperVisor>();
			if (supervisor)
			{

				// perform drop
				if (dragAgent && dropAgent) {
					AIngine::Agent* dragSource = dragAgent;
					supervisor->AddAgent(dragSource);
					__debugbreak();
					dropAgent = nullptr;
				}

				std::stringstream label;

				DisplayTitle(supervisor, "AgentSupervisor");

				ImGui::Selectable("DropArea");
				// Begin Drop. Remember our target to use it AFTER iterating
				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragAgent")) {
						IM_ASSERT(payload->DataSize == sizeof(AIngine::Agent));
						AIngine::Agent* source = dragAgent;
						if (source) {
							dropAgent = supervisor;
						}
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::NewLine();

				ImGui::DragInt("Observations Size", &supervisor->ObservationsSize);
				ImGui::DragInt("Step Rate", &supervisor->StepRate);
				ImGui::DragInt("Max Steps", &supervisor->MaxSteps);
				ImGui::Checkbox("Exploration", &supervisor->Exploration);


				label.str(std::string());
				label << "Population Size : " << supervisor->m_xcsr->populationSize();
				ImGui::Text(label.str().c_str());


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

				if (ImGui::Button("Load Population")) {
					static const char* filterList = "txt,json";
					std::string path;
					AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::OpenFile(filterList, &path, "");

					if (result == AIngine::Util::Filesystem::Result::OKAY)
					{
						supervisor->m_xcsr->loadPopulationCSV(path, true);
					}
				}

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


				ImGui::NewLine();

				auto constants = supervisor->GetConstants();

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

				ImGui::Checkbox("doActionMutation", &constants.doActionMutation);
				ImGui::Checkbox("doActionSetSubsumption", &constants.doActionSetSubsumption);
				ImGui::Checkbox("doGASubsumption", &constants.doGASubsumption);
				ImGui::Checkbox("Use MAM", &constants.useMAM);

				ImGui::NewLine();


				ImGui::Separator();

			}
		}
	}


	void AgentComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			AIngine::GameObject* obj = m_activeGameObjects[0];
			using Agent = AIngine::Agent;
			Agent* agent = nullptr;
			for (auto& comp : obj->GetComponents()) {
				if (dynamic_cast<Agent*>(comp))
					agent = dynamic_cast<Agent*>(comp);
			}
			if (agent)
			{
				//DisplayTitle(agent, "Agent");
				ImGui::Selectable("Agent");

				dragAgent = agent;
				ImGuiDragDropFlags target_flags = 0;
				target_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers | ImGuiDragDropFlags_SourceAllowNullID;
				if (ImGui::BeginDragDropSource(target_flags)) {
					ImGui::SetDragDropPayload("DragAgent", dragAgent, sizeof(AIngine::Agent));
					ImGui::EndDragDropSource();
				}

			}
		}
	}
}