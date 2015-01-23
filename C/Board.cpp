#include "Board.h"
#include "Cell.h"
#include "Game.h"


Board::Board(Game *game)
{
	this->game = game;
	cells = 0;
	allDirections = 0;
}


Board::~Board()
{
	if (cells != 0)
		delete [] cells;

	if (allDirections != 0)
		delete[] allDirections;
}
