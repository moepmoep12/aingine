#include "AIngine/Sounds.h"
#define CUTE_SOUND_IMPLEMENTATION

#include "cute_sound.h"
#include <filesystem>

cs_context_t* AIngine::SoundEngine::SoundContext = nullptr;

AIngine::Sound::Sound(AIngine::Assets::SoundAsset& soundAsset)
	: m_path(soundAsset.path)
{
	m_soundDef = cs_make_def(&soundAsset.GetLoadedSound());
	ExtractName();
}

// Copy Constructor
AIngine::Sound::Sound(const Sound & other)
	: m_path(other.m_path), m_name(other.m_name), m_soundDef(other.m_soundDef)
{
}

// copy assignment
AIngine::Sound & AIngine::Sound::operator=(const Sound & other)
{
	if (&other != this) {
		Stop();
		if (m_playingSound)
			m_playingSound = nullptr;

		m_path = other.m_path;
		m_name = other.m_name;
		m_soundDef = other.m_soundDef;
	}
	return *this;
}

// move constructor
AIngine::Sound::Sound(Sound && other)
	: m_path(other.m_path), m_name(other.m_name), m_soundDef(other.m_soundDef)
{
	m_playingSound = other.m_playingSound;
	other.m_playingSound = nullptr;
}

// move assignment
AIngine::Sound & AIngine::Sound::operator=(Sound && other)
{
	if (&other != this) {
		Stop();
		m_playingSound = other.m_playingSound;
		other.m_playingSound = nullptr;
		m_path = other.m_path;
		m_name = other.m_name;
		m_soundDef = other.m_soundDef;
	}
	return *this;
}

float AIngine::Sound::DurationSeconds() const
{
	return 0.0f;
}

void AIngine::Sound::Play()
{
	// unpause
	if (m_playingSound && m_playingSound->paused)
		cs_pause_sound(m_playingSound, 0);
	else
		m_playingSound = cs_play_sound(AIngine::SoundEngine::SoundContext, m_soundDef);
}

void AIngine::Sound::Pause()
{
	if (m_playingSound && !m_playingSound->paused)
		cs_pause_sound(m_playingSound, 1);
}

void AIngine::Sound::Stop()
{
	if (m_playingSound && m_playingSound->active) {
		cs_stop_sound(m_playingSound);
		m_playingSound = nullptr;
	}
}

void AIngine::Sound::ExtractName()
{
	std::string Path(std::filesystem::canonical(m_path).string());
	size_t first = Path.find_last_of('\\') + 1;
	size_t last = Path.find_last_of('.');
	m_name = Path.substr(first, last - first).c_str();
}

/*********************************** SOUND ENGINE *************************************************************/

AIngine::SoundEngine::SoundEngine(HWND windowHandle)
	: m_windowHandle(windowHandle), Layer("SoundEngine")
{
}

AIngine::SoundEngine::~SoundEngine()
{
	if (SoundContext)
		cs_shutdown_context(SoundContext);
}

void AIngine::SoundEngine::OnAttach()
{
	SoundContext = cs_make_context((void*)m_windowHandle, m_frequenzy, m_latenzy_in_Hz, m_buffered_seconds, m_num_elements_in_playing_pool);
	cs_spawn_mix_thread(SoundContext);
	cs_thread_sleep_delay(SoundContext, 10);
}

void AIngine::SoundEngine::OnDetach()
{
	cs_shutdown_context(SoundContext);
	SoundContext = nullptr;
}
