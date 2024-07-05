#include <iostream>

#include <SDL.h>

#include "Game.h"

#undef main

int main() {
	Game game;

	bool success = game.Initialize();

	if (success) {
		game.RunLoop();
	}

	game.Shutdown();

	return 0;
}