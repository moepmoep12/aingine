#pragma once
#include "AIngine/Component.h"
#include "AIngine/Sounds.h"
#ifdef EDITOR
#include "Editor/Editor.h"
#endif
#include <vector>

namespace AIngine::Editor {
	class SoundComponentWidget;
}

namespace AIngine {
	class SoundComponent final : public Component {

		friend class AIngine::Editor::SoundComponentWidget;

	public:
		SoundComponent();
		virtual ~SoundComponent();

		void Play(int index);
		void Play(const std::string& name);

		void Pause(int index);
		void Pause(const std::string& name);

		void Stop(int index);
		void Stop(const std::string& name);

		inline const std::vector<AIngine::Sound>& GetSounds() const { return m_sounds; }

		void AddSound(const AIngine::Sound& sound);
		AIngine::Sound* GetSound(int index);
		AIngine::Sound* GetSound(const std::string& name);
		

	private:
		std::vector<AIngine::Sound> m_sounds;

		virtual Component* Copy(GameObject* const owner) const override;


#ifdef EDITOR
		AIngine::Editor::Editor::PauseGameEventHandler OnPauseGameEventHandler;
		AIngine::Editor::Editor::ResumeGameEventHandler OnResumeGameEventHandler;
		void OnPauseGame();
		void OnResumeGame();
#endif
	};
}