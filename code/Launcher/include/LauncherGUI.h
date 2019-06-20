#pragma once
#include "Structures/Layer.h"

namespace ProjectLauncher {

	class LauncherGUI : public AIngine::Structures::Layer {
	public:
		LauncherGUI() : Layer("LauncherGUI") {}
		virtual void OnImGuiRender() override;

	private:
		void CreateDockSpace(bool show);
	};
}