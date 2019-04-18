#pragma once

// the Application created wih AIngine implements the function
extern AIngine::Application* AIngine::CreateApplication();

int main(int argc, char** argv) 
{
	AIngine::Log::Init();
	CORE_INFO("Initalized Logging");

	auto app = AIngine::CreateApplication();
	
	CORE_INFO("Created Application");

	app->Run();
	delete app;

	spdlog::drop_all();
	spdlog::shutdown();

	return 0;
}