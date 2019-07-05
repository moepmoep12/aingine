#include "UI/Slider.h"
#include "AIngine/Input.h"
#include "Rendering/shader.h"
#include "Assets/Assets.h"

#include <algorithm>

namespace AIngine::UI {

	Slider::~Slider()
	{
		//if (GetOwner()->GetComponent<SliderHandle>())
		//	GetOwner()->RemoveComponent(m_sliderHandle);
	}

	bool Slider::Render(AIngine::Rendering::GLShaderProgram & shader) const
	{
		auto rect = GetRectangleNative();
		if (m_sliderHandle) {
			glm::vec2 pos = rect.GetPosition();
			// the handle has a width of 5% of the background width
			m_sliderHandle->SetWidth(rect.width * 0.05f);

			pos.x = std::clamp(pos.x + (Value * rect.width) - m_sliderHandle->GetRectangle().width * 0.5f, pos.x, pos.x + rect.width - m_sliderHandle->GetRectangle().width * 0.5f);
			pos.y -= 0.15f * rect.height;
			m_sliderHandle->SetHeight(rect.height + rect.height * 2 * 0.15f);
			m_sliderHandle->SetPosition(pos);
		}

		if (TextComponent) {
			glm::vec2 pos = rect.GetPosition();
			pos.y -= m_sliderHandle->GetRectangle().height;
			TextComponent->SetPosition(pos);
			TextComponent->SetHeight(m_sliderHandle->GetRectangle().height);
			TextComponent->SetWidth(rect.width);
		}

		TextureBackGround.Bind();
		shader.SetVector4f(5 /*spriteColor*/, TintColor);

		return true;
	}

	void Slider::PostInit()
	{
		m_sliderHandle = GetOwner()->AddComponent<SliderHandle>();
		m_sliderHandle->m_slider = this;
		TextureBackGround.Generate(AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>("AIngine/textures/SliderArea.png")->GetBitmap());
		TextComponent = GetOwner()->AddComponent<UIText>();
	}

	Component * Slider::Copy(GameObject * const owner) const
	{
		return nullptr;
	}

	/*----------------------------------------- SLIDERHANDLE ----------------------------------------------*/

	SliderHandle::SliderHandle()
	{
		TextureSlider.Generate(AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>("AIngine/textures/White.png")->GetBitmap());
	}

	void SliderHandle::OnClicked()
	{
		m_isDragging = true;
		auto positionPair = AIngine::Input::GetMousePosition();
		m_lastPosition = glm::vec2(positionPair.first, positionPair.second);
	}

	void SliderHandle::OnMouseReleased()
	{
		m_isDragging = false;
	}

	void SliderHandle::Update(float deltatime)
	{
		if (m_isDragging) {
			auto positionPair = AIngine::Input::GetMousePosition();
			glm::vec2 currentMousePos = glm::vec2(positionPair.first, positionPair.second);
			float delta = currentMousePos.x - m_lastPosition.x;

			float deltaPercent = delta / m_slider->GetRectangle().width;

			m_slider->Value = std::clamp(m_slider->Value + deltaPercent, 0.0f, 1.0f);
			m_slider->OnValueChangedEvent(m_slider->Min + m_slider->Value * m_slider->Max);

			m_lastPosition = currentMousePos;
		}
	}

	bool SliderHandle::Render(AIngine::Rendering::GLShaderProgram & shader) const
	{
		TextureSlider.Bind();
		glm::vec4 Color = TintColor;
		if (m_isDragging) {
			Color = DragColor;
		}
		shader.SetVector4f(5 /*spriteColor*/, Color);
		return true;
	}
}

// initialize EventHandler for OnValueChangedEvent
int AIngine::Events::EventHandler<void, float>::counter = 0;