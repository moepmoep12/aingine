#pragma once
#include "AIngine/Component.h"
#include "AIngine/Sounds.h"

#include <vector>

namespace AIngine::Editor {
	class SoundComponentWidget;
}

namespace AIngine {
	class SoundComponent : public Component {

		friend class AIngine::Editor::SoundComponentWidget;

	public:
		SoundComponent(GameObject* owner);
		virtual ~SoundComponent();

		void Play(int index);
		void Play(const std::string& name);

		void Pause(int index);
		void Pause(const std::string& name);

		void Stop(int index);
		void Stop(const std::string& name);

		inline const std::vector<AIngine::Sound>& GetSounds() const { return m_sounds; }

		void AddSound(const AIngine::Sound& sound);

	private:
		std::vector<AIngine::Sound> m_sounds;
	};
}