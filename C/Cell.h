#pragma once

#include <map>

class Piece;

#define CELL_REF_LENGTH 10

class Cell
{
public:
	Cell();
	~Cell();

	void AddLink(unsigned int dir, Cell* destination);
	Cell* FollowLink(unsigned int dir);
private:
	char reference[CELL_REF_LENGTH];

	Piece *piece;
	std::map<unsigned int, Cell*> links;
	
	int coordX, coordY;

	friend class MoveStep;
	friend class GameParser;
};

