#pragma once

#include "Definitions.h"
#include "MoveStep.h"
#include "PieceType.h"

class Cell;
class GameState;
class Player;
class Piece;

#define MAX_NOTATION_DETAIL 3
#define APPEND_NOTATION_LENGTH 6
#define MOVE_DESCRIPTION_LENGTH TYPE_NAME_LENGTH + 11

class Move
{
public:
	Move(Player *player, GameState *prevState, Piece *piece, Cell *startPos);
	~Move();

	void AddStep(MoveStep *step, bool setNumber = true) { steps.push_back(step); if (setNumber) step->number = steps.size(); }

	Move *Clone();
	GameState *Perform(bool updateDisplay);
	bool Reverse(bool updateDisplay);

	const char *DetermineNotation(int detailLevel);
	const char *GetNotation() { return notation.c_str(); }
	void SetNotation(std::string notation, bool custom = false) { this->notation = notation; if (custom) hasCustomNotation = true; }
	void SetAppendNotation(const char *append);
	const char *GetDescription() { return description.c_str(); }
	void SetDescription(std::string description) { this->description = description; }

	void AddPieceReference(Piece *piece, const char *ref);
	Piece *GetPieceByReference(const char *ref);

	Cell *GetStartPos() { return startPos; }
	Cell *GetEndPos();
	std::list<Piece*> *GetAllCaptures();
	bool IsCapture();
	bool WouldCapture(Cell* target);
	PieceType *GetPromotionType();
	Player *GetPlayer() { return player; }
	Piece *GetPiece() { return piece; }
	GameState *GetPrevState() { return prevState; }
	std::list<MoveStep*> GetSteps() { return steps; }
	bool HasSteps() { return !steps.empty(); }

private:
	Player *player;
	GameState *prevState;
	Piece *piece;
	Cell *startPos;
	std::string notation, description;
	char appendNotation[APPEND_NOTATION_LENGTH];
	bool hasCustomNotation;

	std::list<MoveStep*> steps;
	std::map<const char*, Piece*, char_cmp> references;

	int prevPieceMoveTurn;

	friend class GameState;
};

