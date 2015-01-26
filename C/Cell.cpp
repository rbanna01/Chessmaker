#include "Cell.h"

Cell::Cell()
{
}


Cell::~Cell()
{
}


void Cell::AddLink(unsigned int dir, Cell* destination)
{
	links.insert(std::pair<unsigned int, Cell*>(dir, destination));
}


Cell* Cell::FollowLink(unsigned int dir)
{
	std::map<unsigned int, Cell*>::iterator it = links.find(dir);
	return it == links.end() ? 0 : it->second;
}