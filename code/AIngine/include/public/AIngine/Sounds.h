#pragma once

#include "Structures/Layer.h"
#include "Assets/Assets.h"
#include "cute_sound.h"

#include <mmsystem.h>
#include <dsound.h>

namespace AIngine {
	class Sounds : public AIngine::Structures::Layer {
	public:

		virtual ~Sounds();

		// inherited via Layer
		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnUpdate(float delta) {}
		virtual void OnEvent(AIngine::Events::Event& e) {}
		virtual void OnImGuiRender() {}

		static void Play(AIngine::Assets::SoundAsset& soundAsset);

	private:
		static cs_context_t* m_context;
		const unsigned int m_frequenzy = 44000;
		const unsigned int m_latenzy_in_Hz = 15;
		const unsigned int m_buffered_seconds = 5;
		const unsigned int m_num_elements_in_playing_pool = 5;
	};
}