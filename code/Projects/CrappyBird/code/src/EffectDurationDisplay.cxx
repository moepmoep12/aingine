#include "EffectDurationDisplay.h"
namespace CrappyBird {

	// Constructor
	EffectDurationDisplay::EffectDurationDisplay()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void EffectDurationDisplay::OnStart()
	{

	}

	// End is called when gameplay ends for this script
	void EffectDurationDisplay::OnEnd()
	{
	}

	// Update is called once per frame
	void EffectDurationDisplay::Update(float delta)
	{
		Remaining -= delta;
		if (Remaining < 0) {
			GetOwner()->SetActive(false);
			return;
		}
		float sec;
		float mil;
		mil = std::modf(Remaining, &sec);
		mil *= 100;
		std::stringstream txt;
		if (sec < 10)
			txt << "0";
		txt << (int)sec << " : ";
		if (mil < 10)
			txt << "0";
		txt << (int)mil;
		m_durationText->Text = txt.str();
	}

	// Callback for events
	void EffectDurationDisplay::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void EffectDurationDisplay::SetYPos(int ypos)
	{
		if (!m_effectImage) {
			m_effectImage = GetOwner()->GetChildAt(0)->GetComponent < AIngine::UI::Image>();
			m_backGroundImage = GetComponent < AIngine::UI::Image>();
			m_durationText = GetOwner()->GetChildAt(0)->GetComponent < AIngine::UI::UIText>();
		}

		m_effectImage->SetPosition(glm::vec2(m_effectImage->GetRectangleNative().x, ypos));
		m_backGroundImage->SetPosition(glm::vec2(m_effectImage->GetRectangleNative().x, ypos));
		m_durationText->SetPosition(glm::vec2(m_effectImage->GetRectangleNative().x, ypos));
	}

	void EffectDurationDisplay::Activate(float remaining)
	{
		if (!m_effectImage) {
			m_effectImage = GetOwner()->GetChildAt(0)->GetComponent < AIngine::UI::Image>();
			m_backGroundImage = GetComponent < AIngine::UI::Image>();
			m_durationText = GetOwner()->GetChildAt(0)->GetComponent < AIngine::UI::UIText>();
		}

		GetOwner()->SetActive(true);
		Remaining = remaining;
	}
}