#include "Game.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char** argv)
{
	Game* game = new Game;
	game->start();
	game->run();
	game->stop();
	delete game; 
	return 0;
}