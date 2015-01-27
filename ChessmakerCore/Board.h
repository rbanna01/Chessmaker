#pragma once

#include <map>

class Cell;
class Game;

typedef std::map<unsigned int, unsigned int> relativeDir_t;
typedef std::pair<unsigned int, unsigned int> relativeDirValue_t;

class Board
{
public:
	Board(Game *game);
	~Board();

	unsigned int ResolveDirections(unsigned int dir, unsigned int prevDir);

private:
	unsigned int ResolveRelativeDirection(unsigned int id, unsigned int relativeTo);

	Game *game;
	Cell *cells;
	unsigned int allAbsoluteDirections, firstRelativeDirection, lastRelativeDirection;
	std::map<unsigned int, relativeDir_t> relativeDirections;

	friend class GameParser;
};

