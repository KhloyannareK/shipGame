#include "shipGame.h"

using namespace myShipGame;
int main() {
	try {
		shipGame game{ 20 };
		shipGame::rulesOfTheGame();
		game.startGame();
	}
	catch (shipGameException& ex) {
		ex.what();
	}

	return 0;
}