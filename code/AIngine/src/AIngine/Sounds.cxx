#include "AIngine/Sounds.h"
#define CUTE_SOUND_IMPLEMENTATION
#include "cute_sound.h"
#include "Application.h"

cs_context_t* AIngine::Sounds::m_context = nullptr;

AIngine::Sounds::~Sounds()
{
	if (m_context)
		cs_shutdown_context(m_context);
}

void AIngine::Sounds::OnAttach()
{
	AIngine::Application::Get().GetWindow().GetNativeWindow();
	m_context = cs_make_context((void*)AIngine::Application::Get().GetWindow().GetWin32Window(), m_frequenzy, m_latenzy_in_Hz, m_buffered_seconds, m_num_elements_in_playing_pool);
	cs_spawn_mix_thread(m_context);
	cs_thread_sleep_delay(m_context, 10);
}

void AIngine::Sounds::OnDetach()
{
	cs_shutdown_context(m_context);
	m_context = nullptr;
}

void AIngine::Sounds::Play(AIngine::Assets::SoundAsset & soundAsset)
{
	cs_play_sound(m_context, soundAsset.GetSound());
}
