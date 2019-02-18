#pragma once

namespace AIngine {

	class Application {

	public:
		Application();
		virtual ~Application();

		void Run();

	};

	// to be defined by the actual Application
	Application* CreateApplication();
}