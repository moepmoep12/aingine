#pragma once

#include "Layer.h"

namespace AIngine::UI {

	class ImGuiLayer : public AIngine::Layer {

	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(AIngine::Events::Event& e) override;

	};
}