#pragma once

#include <map>

class Piece;

#define CELL_REF_LENGTH 10

class Cell
{
public:
	Cell();
	~Cell();

	void AddLink(int dir, Cell* destination);
	Cell* FollowLink(int dir);
private:
	char reference[CELL_REF_LENGTH];

	Piece *piece;
	std::map<int, Cell*> links;
	
	int coordX, coordY;

	friend class MoveStep;
	friend class GameParser;
};

