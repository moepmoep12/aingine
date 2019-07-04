#pragma once
#include "UI/UIELement.h"
#include "Events/Event.h"
#include "Rendering/texture.h"

namespace AIngine::UI {

	class SliderHandle;

	class Slider : public UIElement {
	public:
		virtual ~Slider();

		virtual bool Render(AIngine::Rendering::GLShaderProgram& shader) const override;
		virtual void PostInit();

	public:
		AIngine::Rendering::Texture2D TextureBackGround;

		float Min = 0;
		float Max = 1;
		float Value = 0.5;

		AIngine::Events::Event<void, float> OnValueChangedEvent;
		typedef AIngine::Events::EventHandler<void, float> OnValueChangedEventHandler;

		SliderHandle* m_sliderHandle;
	protected:
		virtual Component* Copy(GameObject* const owner) const override;

	private:
	};

	class SliderHandle : public UIElement {
	public:
		SliderHandle();
		virtual void OnClicked() override;
		virtual void OnMouseReleased() override;
		virtual void Update(float deltatime) override;

		virtual bool Render(AIngine::Rendering::GLShaderProgram& shader) const override;

	public:
		glm::vec4 DragColor = glm::vec4(0.97, 1, 0.4, 1);
		AIngine::Rendering::Texture2D TextureSlider;
		Slider* m_slider;

	private:
		bool m_isDragging = false;
		glm::vec2 m_lastPosition;
	};
}