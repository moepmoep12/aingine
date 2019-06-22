#pragma once

// the Application created wih AIngine implements the function
extern AIngine::Application* AIngine::CreateApplication();
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
int main(int argc, char** argv) 
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

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