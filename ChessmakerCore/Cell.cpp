#include "Cell.h"

Cell::Cell()
{
	piece = 0;
}


Cell::~Cell()
{
}


void Cell::AddLink(direction_t dir, Cell* destination)
{
	links.insert(std::pair<direction_t, Cell*>(dir, destination));
}


Cell* Cell::FollowLink(direction_t dir)
{
	auto it = links.find(dir);
	return it == links.end() ? 0 : it->second;
}


int Cell::GetMaxDist(direction_t dir)
{
	auto it = maxDists.find(dir);
	return it == maxDists.end() ? 0 : it->second;
}