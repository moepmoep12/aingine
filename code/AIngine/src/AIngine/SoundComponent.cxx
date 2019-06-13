#include "AIngine/SoundComponent.h"
#include "AIngine/Sounds.h"

namespace AIngine {
	AIngine::SoundComponent::SoundComponent(GameObject * owner)
		: Component(owner)
	{
	}

	AIngine::SoundComponent::~SoundComponent()
	{
		for (auto& sound : m_sounds) {
			if (sound.IsPlaying())
				sound.Stop();
		}
		m_sounds.clear();
	}

	void SoundComponent::Play(int index)
	{
		if (index < m_sounds.size()) {
			m_sounds[index].Play();
		}
	}

	void SoundComponent::Play(const std::string & name)
	{
		for (auto& it = m_sounds.begin(); it != m_sounds.end(); it++) {
			if ((*it._Ptr).GetName() == name) {
				(*it._Ptr).Play();
				break;
			}
		}
	}

	void SoundComponent::Pause(int index)
	{
		if (index < m_sounds.size()) {
			m_sounds[index].Pause();
		}
	}

	void SoundComponent::Pause(const std::string & name)
	{
		for (auto& it = m_sounds.begin(); it != m_sounds.end(); it++) {
			if ((*it._Ptr).GetName() == name) {
				(*it._Ptr).Pause();
				break;
			}
		}
	}

	void SoundComponent::Stop(int index)
	{
		if (index < m_sounds.size()) {
			m_sounds[index].Stop();
		}
	}

	void SoundComponent::Stop(const std::string & name)
	{
		for (auto& it = m_sounds.begin(); it != m_sounds.end(); it++) {
			if ((*it._Ptr).GetName() == name) {
				(*it._Ptr).Stop();
				break;
			}
		}
	}

	void SoundComponent::AddSound(const AIngine::Sound & sound)
	{
		m_sounds.push_back(sound);
	}

	AIngine::Sound * AIngine::SoundComponent::GetSound(int index)
	{
		if (index < m_sounds.size())
			return &m_sounds[index];
		else return nullptr;
	}

	AIngine::Sound * AIngine::SoundComponent::GetSound(const std::string & name)
	{
		for (auto& it = m_sounds.begin(); it != m_sounds.end(); it++) {
			if ((*it._Ptr).GetName() == name) {
				return (&*it._Ptr);
			}
		}
		return nullptr;
	}

}