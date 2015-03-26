#include "Board.h"
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


direction_t Board::ResolveDirections(direction_t dir, direction_t prevDir)
{
	if (dir == DIRECTION_SAME)
		return prevDir;
	
	if (dir < firstRelativeDirection)
		return dir; // a single absolute dir, or a group of them.

	// must resolve at least one relative direction
	direction_t absoluteOnly = dir & allAbsoluteDirections;
	for (direction_t rel = firstRelativeDirection; rel <= lastRelativeDirection; rel = rel << 1)
		if ((dir & rel) == rel)
			absoluteOnly |= ResolveRelativeDirection(rel, prevDir);
	
	return absoluteOnly;
}


direction_t Board::ResolveRelativeDirection(direction_t id, direction_t relativeTo)
{
	std::map<direction_t, relativeDir_t>::iterator it = relativeDirections.find(id);

	if (it == relativeDirections.end())
		return 0; // this isn't a relative direction
	relativeDir_t relDir = it->second;

	relativeDir_t::iterator it2 = relDir.find(relativeTo);
	if (it2 == relDir.end())
		return 0; // this relative direction has no entry for this "from" value

	return it2->second;
}


int Board::GetMaxDistance(Cell *cell, direction_t direction)
{
	int num = 0;
	
	std::set<Cell*> alreadyVisited;
	alreadyVisited.insert(cell);
	
	cell = cell->FollowLink(direction);
	while (cell != 0)
	{
		num++;

		// without this bit, circular boards would infinite loop
		if (alreadyVisited.find(cell) != alreadyVisited.end())
			break;

		alreadyVisited.insert(cell);
		cell = cell->FollowLink(direction);
	}

	return num;
}