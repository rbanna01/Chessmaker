#pragma once

#include "Definitions.h"

class Cell;
class Game;

typedef std::map<direction_t, direction_t> relativeDir_t;
typedef std::pair<direction_t, direction_t> relativeDirValue_t;

class Board
{
public:
	Board(Game *game);
	~Board();

	direction_t ResolveDirections(direction_t dir, direction_t prevDir);
	std::vector<Cell*> GetCells() { return cells; }
	void PrecomputeDistances();

private:
	int GetMaxDistance(Cell *cell, direction_t direction);
	direction_t ResolveRelativeDirection(direction_t id, direction_t relativeTo);

	Game *game;
	std::vector<Cell*> cells;
	direction_t allAbsoluteDirections, firstRelativeDirection, lastRelativeDirection;
	std::map<direction_t, relativeDir_t> relativeDirections;

	friend class GameParser;
};
