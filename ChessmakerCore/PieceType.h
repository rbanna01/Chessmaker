#pragma once

#include "Definitions.h"

class MoveDefinition;

#define TYPE_NAME_LENGTH 32
#define TYPE_NOTATION_LENGTH 3

class PieceType
{
public:
	PieceType();
	~PieceType();
	std::list<MoveDefinition*> GetMoves() { return moves; }
private:
	int value = 1;
	char name[TYPE_NAME_LENGTH];
	char notation[TYPE_NOTATION_LENGTH];
	std::list<MoveDefinition*> moves;

#ifndef NO_SVG
	std::map<int, char*> appearances;
#endif
	//std::list<PromotionOpportunity*> promotionOpportunities;
	PieceType *capturedAs;

	friend class GameParser;
	friend class Move;
};

