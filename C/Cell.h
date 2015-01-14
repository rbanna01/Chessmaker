#pragma once
#include <map>

class Piece;

class Cell
{
public:
	Cell();
	~Cell();

	void AddLink(int dir, Cell* destination);
	Cell* FollowLink(int dir);
private:
	static int nextID;
	int uniqueID;

	Piece *piece;
	std::map<int, Cell*> links;
	
	friend class MoveStep;
};

