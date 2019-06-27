#pragma once

#include "Structures/Layer.h"
#include "Assets/Assets.h"
#include "cute_sound.h"

#include <mmsystem.h>
#include <dsound.h>
#include <string>

namespace AIngine {

	class Sound final {
	public:
		Sound(AIngine::Assets::SoundAsset& soundAsset);
		Sound(const Sound& other);
		Sound& operator=(const Sound& other);
		Sound(Sound&& other);
		Sound& operator=(Sound&& other);

		inline bool IsPlaying() const { if (m_playingSound) return m_playingSound->active && !m_playingSound->paused; else return false; }
		inline bool IsPaused() const { if (m_playingSound) return m_playingSound->paused; else return false; }
		inline bool IsLooping() const { return m_soundDef.looped; }
		inline int SampleCount() const { return m_soundDef.loaded->sample_count; }
		inline int SampleRate() const { return m_soundDef.loaded->sample_rate; }
		inline int ChannelCount() const { return m_soundDef.loaded->channel_count; }
		inline float GetPan() const { return m_soundDef.pan; }
		inline float GetPitch() const { return m_soundDef.pitch; }
		inline float GetDelay() const { return m_soundDef.delay; }
		inline float GetProgress() const { if (m_playingSound) return (float)m_playingSound->sample_index / (float)m_playingSound->loaded_sound->sample_count; else return 0; }
		float DurationSeconds() const;
		inline std::string GetName() const { return m_name; }
		inline std::string GetPath() const { return m_path; }
		inline std::pair<float, float> GetVolume() const { return std::pair<float, float>(m_soundDef.volume_left, m_soundDef.volume_right); }

		inline void SetLooping(bool loop) { m_soundDef.looped = loop; }
		inline void SetPan(float pan) { m_soundDef.pan = pan; }
		inline void SetPitch(float pitch) { m_soundDef.pitch = pitch; }
		inline void SetDelay(float delay) { m_soundDef.delay = delay; }
		inline void SetVolume(float left, float right) { m_soundDef.volume_left = left; m_soundDef.volume_right = right; }

		void Play();
		void Pause();
		void Stop();

	private:
		cs_play_sound_def_t m_soundDef;
		cs_playing_sound_t* m_playingSound = nullptr;
		std::string m_name;
		std::string m_path;

		void ExtractName();
	};

	class SoundEngine final : public AIngine::Structures::Layer {
	public:
		SoundEngine(HWND windowHandle);
		virtual ~SoundEngine();

		// inherited via Layer
		virtual void OnAttach();
		virtual void OnDetach();

		static cs_context_t* SoundContext;
	private:
		const unsigned int m_frequenzy = 44000;
		const unsigned int m_latenzy_in_Hz = 15;
		const unsigned int m_buffered_seconds = 5;
		const unsigned int m_num_elements_in_playing_pool = 5;
		HWND m_windowHandle;
	};
}