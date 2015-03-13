#pragma once

#include <map>
#include "Definitions.h"

class Piece;

#define CELL_REF_LENGTH 10

class Cell
{
public:
	Cell();
	~Cell();

	void AddLink(direction_t dir, Cell* destination);
	Cell* FollowLink(direction_t dir);
	char *GetName() { return reference; }
	Piece *GetPiece() { return piece; }
private:
	char reference[CELL_REF_LENGTH];

	Piece *piece;
	std::map<direction_t, Cell*> links;
	
	int coordX, coordY;

	friend class MoveStep;
	friend class GameParser;
};

