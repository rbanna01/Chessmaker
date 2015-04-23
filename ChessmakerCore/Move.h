#pragma once

#include "Definitions.h"

class Cell;
class GameState;
class MoveStep;
class Player;
class Piece;
class PieceType;

#define MAX_NOTATION_DETAIL 3
#define NOTATION_LENGTH 10

class Move
{
public:
	Move(Player *player, GameState *prevState, Piece *piece, Cell *startPos);
	~Move();

	void AddStep(MoveStep *step) { steps.push_back(step); }

	Move *Clone();
	GameState *Perform(bool updateDisplay);
	bool Reverse(bool updateDisplay);

	char *DetermineNotation(int detailLevel);
	char *GetNotation() { return notation; }

	void AddPieceReference(Piece *piece, const char *ref);
	Piece *GetPieceByReference(const char *ref);

	Cell *GetEndPos();
	std::list<Cell*> GetAllPositions();
	bool IsCapture();
	int GetCaptureValue();
	bool WouldCapture(Cell* target);
	PieceType *GetPromotionType();
	Player *GetPlayer() { return player; }
	Piece *GetPiece() { return piece; }
	GameState *GetPrevState() { return prevState; }
	std::list<MoveStep*> GetSteps() { return steps; }
private:
	Player *player;
	GameState *prevState;
	Piece *piece;
	Cell *startPos;
	char notation[NOTATION_LENGTH];

	std::list<MoveStep*> steps;
	std::map<const char*, Piece*, char_cmp> references;

	int prevPieceMoveTurn;

	friend class GameState;
};

