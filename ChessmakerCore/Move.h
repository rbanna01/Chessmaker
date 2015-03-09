#pragma once

#include "Definitions.h"

class Cell;
class GameState;
class MoveStep;
class Player;
class Piece;
class PieceType;

#define MAX_NOTATION_DETAIL 3

class Move
{
public:
	Move(Player *player, GameState *prevState, Piece *piece, Cell *startPos);
	~Move();

	void AddStep(MoveStep *step) { steps.push_back(step); }

	Move *Clone();
	bool Perform(bool updateDisplay);
	bool Reverse(bool updateDisplay);

	char *DetermineNotation(int detailLevel);

	void AddPieceReference(Piece *piece, char *ref);
	Piece *GetPieceByReference(char *ref);

	Cell *GetEndPos();
	std::list<Cell*> GetAllPositions();
	bool IsCapture();
	bool WouldCapture(Cell* target);
	PieceType *GetPromotionType();

private:
	Player *player;
	GameState *prevState, *subsequentState;
	Piece *piece;
	Cell *startPos;
	char notation[10];

	std::list<MoveStep*> steps;
	std::map<char*, Piece*, char_cmp> references;

	int prevPieceMoveTurn;

	friend class GameState;
};

