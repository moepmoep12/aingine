#include "AIngine/SoundComponent.h"
#include "AIngine/Sounds.h"
#ifdef EDITOR
#include "Editor/Editor.h"
#endif
namespace AIngine {

	SoundComponent::SoundComponent()
	{
#ifdef EDITOR
		this->OnPauseGameEventHandler = AIngine::Editor::Editor::PauseGameEventHandler(std::bind(&SoundComponent::OnPauseGame, this));
		this->OnResumeGameEventHandler = AIngine::Editor::Editor::ResumeGameEventHandler(std::bind(&SoundComponent::OnResumeGame, this));
		AIngine::Editor::Editor::PauseGameEvent += this->OnPauseGameEventHandler;
		AIngine::Editor::Editor::ResumeGameEvent += this->OnResumeGameEventHandler;
#endif
	}

	AIngine::SoundComponent::~SoundComponent()
	{
#ifdef EDITOR
		AIngine::Editor::Editor::PauseGameEvent -= this->OnPauseGameEventHandler;
		AIngine::Editor::Editor::ResumeGameEvent -= this->OnResumeGameEventHandler;
#endif
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

	Component * SoundComponent::Copy(GameObject * const owner) const
	{
		SoundComponent* copy = new SoundComponent();
		copy->m_owner = owner;

		copy->SetName(GetName());

		for (auto& sound : m_sounds) {
			copy->m_sounds.push_back(sound);
		}

		return std::move(copy);
	}

#ifdef EDITOR
	void SoundComponent::OnPauseGame()
	{
		for (auto& sound : m_sounds)
			if (sound.IsPlaying()) sound.Pause();
	}

	void SoundComponent::OnResumeGame()
	{
		for (auto& sound : m_sounds)
			if (sound.IsPaused()) sound.Play();
	}
#endif
}