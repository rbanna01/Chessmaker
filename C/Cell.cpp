#include "Cell.h"

Cell::Cell()
{
}


Cell::~Cell()
{
}


void Cell::AddLink(int dir, Cell* destination)
{
	links[dir] = destination;
}


Cell* Cell::FollowLink(int dir)
{
	std::map<int, Cell*>::iterator it = links.find(dir);
	return it == links.end() ? 0 : it->second;
}