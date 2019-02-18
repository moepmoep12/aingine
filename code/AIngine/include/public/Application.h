#pragma once

namespace AIngine {

	class Application {

	public:
		Application();
		virtual ~Application();

		void Run();

	};

	Application* CreateApplication();
}