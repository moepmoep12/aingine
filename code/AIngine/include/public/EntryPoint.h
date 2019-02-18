#pragma once

extern AIngine::Application* AIngine::CreateApplication();

int main(int argc, char** argv) {

	auto app = AIngine::CreateApplication();
	app->Run();
	delete app;


	return 0;
}