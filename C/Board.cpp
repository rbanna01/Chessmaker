#include "Board.h"
#include "Constants.h"
#include "Cell.h"
#include "Game.h"


Board::Board(Game *game)
{
	this->game = game;
	cells = 0;
	allAbsoluteDirections = 0;
}


Board::~Board()
{
	if (cells != 0)
		delete [] cells;
}

unsigned int Board::ResolveDirections(unsigned int dir, unsigned int prevDir)
{
	if (dir == DIRECTION_SAME)
		return prevDir;
	
	if (dir < firstRelativeDirection)
		return dir; // a single absolute dir, or a group of them.

	// must resolve at least one relative direction
	unsigned int absoluteOnly = dir & allAbsoluteDirections;
	for (unsigned int rel = firstRelativeDirection; rel <= lastRelativeDirection; rel << 1)
		if ((dir & rel) == rel)
			absoluteOnly |= ResolveRelativeDirection(rel, prevDir);
	
	return absoluteOnly;
}

unsigned int Board::ResolveRelativeDirection(unsigned int id, unsigned int relativeTo)
{
	std::map<unsigned int, relativeDir_t>::iterator it = relativeDirections.find(id);

	if (it == relativeDirections.end())
		return 0; // this isn't a relative direction
	relativeDir_t relDir = it->second;

	relativeDir_t::iterator it2 = relDir.find(relativeTo);
	if (it2 == relDir.end())
		return 0; // this relative direction has no entry for this "from" value

	return it2->second;
}