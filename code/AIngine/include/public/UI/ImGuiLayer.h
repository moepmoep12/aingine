#pragma once

#include "Layer.h"
#include "LogWidget.h"

namespace AIngine::UI {

	class ImGuiLayer : public AIngine::Layer {

	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void OnBegin();
		void OnEnd();

		static LogWidget s_logWidget;

	private:
		void CreateDockSpace(const bool show);
	};
}