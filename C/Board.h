#pragma once

class Cell;
class Game;

class Board
{
public:
	Board(Game *game);
	~Board();

	int ResolveDirection(int dir, int prevDir) { return 0; } // todo: look at signature for this

private:
	Game *game;
	Cell *cells;
	int *allDirections;

	friend class GameParser;
};

