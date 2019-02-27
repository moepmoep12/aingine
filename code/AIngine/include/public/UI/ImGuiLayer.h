#pragma once

#include "Layer.h"
#include "Events/ApplicationEvents.h"
#include "Events/InputEvents.h"

namespace AIngine::UI {

	class ImGuiLayer : public AIngine::Layer {

	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(AIngine::Events::Event& e) override;

	private:
		bool OnMouseButtonPressedEvent(AIngine::Events::MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(AIngine::Events::MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(AIngine::Events::MouseMovedEvent& e);
		bool OnMouseScrolledEvent(AIngine::Events::MouseScrolledEvent& e);
		bool OnKeyPressedEvent(AIngine::Events::KeyPressedEvent& e);
		bool OnKeyReleasedEvent(AIngine::Events::KeyReleasedEvent& e);
		bool OnKeyTypedEvent(AIngine::Events::KeyTypedEvent& e);
		bool OnWindowResizeEvent(AIngine::Events::WindowResizeEvent& e);

	};
}