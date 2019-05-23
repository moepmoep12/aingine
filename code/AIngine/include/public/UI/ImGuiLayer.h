#pragma once

#include "Structures/Layer.h"


//namespace AIngine::Editor {
//	struct LogWidget;
//}

namespace AIngine::UI {

	class ImGuiLayer : public AIngine::Structures::Layer {

	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void OnBegin();
		void OnEnd();

	private:
		void CreateDockSpace(bool show);
	};
}