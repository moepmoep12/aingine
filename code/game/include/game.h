#pragma once
#include "Core.h"
#include <iostream>

class Game : public AIngine::Application {
public:
	Game() {

	}
	~Game() {
		std::cout << "destructor game \n";
		int x;
		std::cin >> x;
	}
};