#pragma once

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
	int GetMaxDist(direction_t dir);
	char *GetName() { return reference; }
	Piece *GetPiece() { return piece; }
private:
	char reference[CELL_REF_LENGTH];

	Piece *piece;
	std::map<direction_t, Cell*> links;
	std::map<direction_t, int> maxDists;

	friend class Board;
	friend class MoveStep;
	friend class GameParser;
};

