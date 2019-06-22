#include "Editor/Widgets/SoundComponentWidget.h"
#include "AIngine/SoundComponent.h"
#include "AIngine/GameObject.h"
#include "Assets/Assets.h"

#include <nfd.h>


namespace AIngine::Editor {
	SoundComponentWidget::SoundComponentWidget()
	{
		std::string path = "AIngine/textures/Editor/add-1.png";
		AIngine::Rendering::Bitmap* bitmap = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(path)->GetBitmap();
		m_Icons["add"] = AIngine::Rendering::Texture2D(*bitmap);

		// playIcon
		path = "AIngine/textures/Editor/play.png";
		bitmap = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(path)->GetBitmap();
		m_Icons["play"] = AIngine::Rendering::Texture2D(*bitmap);

		// pauseIcon
		path = "AIngine/textures/Editor/pause.png";
		bitmap = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(path)->GetBitmap();
		m_Icons["pause"] = AIngine::Rendering::Texture2D(*bitmap);

		// stopIcon
		path = "AIngine/textures/Editor/stop.png";
		bitmap = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(path)->GetBitmap();
		m_Icons["stop"] = AIngine::Rendering::Texture2D(*bitmap);
	}

	SoundComponentWidget::~SoundComponentWidget()
	{
	}

	void AIngine::Editor::SoundComponentWidget::OnImGuiRender()
	{
		ImGui::Separator();
		ImGui::NewLine();
		if (m_activeGameObjects.size() == 1) {
			AIngine::GameObject* obj = m_activeGameObjects[0];
			AIngine::SoundComponent* soundComp = obj->GetComponent<AIngine::SoundComponent>();

			ImGui::Separator();
			// Title
			float textWidth = ImGui::CalcTextSize("Sound Component").x;
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) * 0.5f);
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Sound Component");
			if (ImGui::BeginPopupContextItem("Sound Component")) {
				if (ImGui::Selectable("Delete##physComp")) {
					obj->RemoveComponent<AIngine::SoundComponent>();
					ImGui::EndPopup();
					return;
				}
				ImGui::EndPopup();
			}
			ImGui::NewLine();

			std::vector<AIngine::Sound>::const_iterator toRemove;
			bool remove = false;

			for (auto& it = soundComp->m_sounds.begin(); it != soundComp->m_sounds.end(); it++)
			{
				AIngine::Sound* sound = &(*it._Ptr);

				// Title
				ImGui::TextColored(ImVec4(0.098f, 0.325f, 0.82f, 1), sound->GetName().c_str());
				if (ImGui::BeginPopupContextItem("SoundContext")) {
					if (ImGui::Selectable("Remove##sound")) {
						toRemove = it;
						remove = true;
						ImGui::EndPopup();
						continue;
					}
					ImGui::EndPopup();
				}
				ImGui::NewLine();

				// info about the loaded sound, not changeable
				ImGui::Columns(2);
				static const ImVec4 textValueColor = ImVec4(0.34f, 0.448f, 0.33f, 1);
				ImGui::TextColored(textValueColor, (std::to_string(sound->DurationSeconds()) + "s").c_str()); ImGui::NextColumn(); ImGui::Text("Duration"); ImGui::NextColumn();
				ImGui::TextColored(textValueColor, (std::to_string(sound->SampleRate())).c_str()); ImGui::NextColumn(); ImGui::Text("Sample Rate"); ImGui::NextColumn();
				ImGui::TextColored(textValueColor, (std::to_string(sound->ChannelCount())).c_str()); ImGui::NextColumn(); ImGui::Text("Channels"); ImGui::NextColumn();
				ImGui::Columns(1);
				ImGui::NewLine();

				// Delay
				float delay = sound->GetDelay();
				std::stringstream ss;
				ss << "Delay" << "##" << sound->GetPath();
				if (ImGui::DragFloat(ss.str().c_str(), &delay, 0.1f)) {
					sound->SetDelay(delay);
				}
				// Looping
				bool looped = sound->IsLooping();
				ss.str(std::string());
				ss << "Looping" << "##" << sound->GetPath();
				if (ImGui::Checkbox(ss.str().c_str(), &looped)) {
					sound->SetLooping(looped);
				}
				// Volume
				float volumes[] = { sound->GetVolume().first , sound->GetVolume().second };
				ss.str(std::string());
				ss << "Volume left/right" << "##" << sound->GetPath();
				if (ImGui::DragFloat2(ss.str().c_str(), volumes)) {
					sound->SetVolume(volumes[0], volumes[1]);
				}
				// Pan
				float pan = sound->GetPan();
				ss.str(std::string());
				ss << "Pan" << "##" << sound->GetPath();
				if (ImGui::SliderFloat(ss.str().c_str(), &pan, 0, pan * 5, "%.1f")) {
					sound->SetPan(pan);
				}
				// Pitch
				float pitch = sound->GetPitch();
				ss.str(std::string());
				ss << "Pitch" << "##" << sound->GetPath();
				if (ImGui::SliderFloat(ss.str().c_str(), &pitch, 0, pitch * 5, "%.1f")) {
					sound->SetPitch(pitch);
				}

				ImGui::NewLine();

				// pause button
				if (sound->IsPlaying()) {
					ss.str(std::string());
					ss << "Pause" << "##" << sound->GetPath();
					if (ImGui::ImageButtonWithText((ImTextureID)m_Icons["pause"].ID, ss.str().c_str())) {
						sound->Pause();
					}
				}
				// play button
				else {
					ss.str(std::string());
					ss << "Play" << "##" << sound->GetPath();
					if (ImGui::ImageButtonWithText((ImTextureID)m_Icons["play"].ID, ss.str().c_str())) {
						sound->Play();
					}
				}

				ImGui::SameLine();
				// stop button
				ss.str(std::string());
				ss << "Stop" << "##" << sound->GetPath();
				if (ImGui::ImageButtonWithText((ImTextureID)m_Icons["stop"].ID, ss.str().c_str())) {
					if (sound->IsPlaying()) sound->Stop();
				}

				ImGui::SameLine();
				// progress bar
				ImGui::ProgressBar(sound->GetProgress());

				ImGui::Separator();
			}


			if (ImGui::ImageButtonWithText((ImTextureID)m_Icons["add"].ID, "Add Sound##addSoundWidget"))
			{
				static const nfdchar_t *filterList = "wav";
				nfdchar_t *outPath = NULL;
				nfdresult_t result = NFD_OpenDialog(filterList, NULL, &outPath);

				if (result == NFD_OKAY)
				{
					AIngine::Assets::SoundAsset& sound = *AIngine::Assets::AssetRegistry::Load<AIngine::Assets::SoundAsset>(outPath);
					soundComp->m_sounds.push_back(AIngine::Sound(sound));
					free(outPath);
				}
			}

			if (remove) {
				soundComp->m_sounds.erase(toRemove);
			}
		}
		ImGui::Separator();
	}
}
