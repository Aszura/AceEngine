#include "Game.h"

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <string>

int main(int argc, char** argv)
{
	//Change current running path to executable's path
	std::string execPath(argv[0]);
	std::string execDirectory = execPath.substr(0, execPath.find_last_of("\\/"));
#ifdef _WIN32
	int i =_chdir(execDirectory.c_str());
#else
	chdir(execDirectory.c_str());
#endif

	Game* game = new Game;
	game->start();
	game->run();
	game->stop();
	delete game; 
	return 0;
}