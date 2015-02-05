#include "EndOfGame.h"

EndOfGame::EndOfGame()
{
	illegalMovesSpecified = false;
}


EndOfGame::~EndOfGame()
{
	while (!startOfTurnChecks.empty())
		delete startOfTurnChecks.front(), startOfTurnChecks.pop_front();

	while (!endOfTurnChecks.empty())
		delete endOfTurnChecks.front(), endOfTurnChecks.pop_front();
}


EndOfGame *EndOfGame::CreateDefault()
{
	// todo: implement this
	return 0;
}