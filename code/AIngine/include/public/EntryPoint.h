#pragma once

// the Application created wih AIngine implements the function
extern AIngine::Application* AIngine::CreateApplication();

int main(int argc, char** argv) 
{
	AIngine::Log::Init();
	CORE_ERROR(" Engine core detected an error");
	DEBUG_WARN(" Debug warning");


	auto app = AIngine::CreateApplication();
	app->Run();
	delete app;


	return 0;
}