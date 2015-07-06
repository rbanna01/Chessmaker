#pragma once

#include "Definitions.h"

class MoveDefinition;
class Player;

#define TYPE_NAME_LENGTH 32
#define TYPE_NOTATION_LENGTH 3

class PieceType
{
public:
	PieceType();
	~PieceType();
	std::list<MoveDefinition*> &GetMoves() { return moves; }
	const char *GetName() { return name; }
	const char *GetNotation() { return notation; }
	int GetValue() { return value; }
#ifndef NO_SVG
	const char *GetAppearance(Player *player);
#endif

private:
	int value = 1;
	char name[TYPE_NAME_LENGTH];
	char notation[TYPE_NOTATION_LENGTH];
	std::list<MoveDefinition*> moves;
	PieceType *capturedAs;

	friend class GameParser;
	friend class Move;
};

