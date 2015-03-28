#pragma once

#include "Definitions.h"

class EndOfGameCheck;
class GameState;
class Move;
class StateConditionGroup;


class EndOfGame
{
public:
	EndOfGame();
	~EndOfGame();

	typedef enum { None, Win, Lose, Draw, IllegalMove } CheckType_t;

	static EndOfGame *CreateDefault();

	CheckType_t CheckStartOfTurn(GameState *state, bool canMove);
	CheckType_t CheckEndOfTurn(GameState *state, Move *move);
	bool AnyIllegalMovesSpecified() { return illegalMovesSpecified; }

private:
	bool illegalMovesSpecified;

	std::list<EndOfGameCheck*> startOfTurnChecks, endOfTurnChecks;

	friend class GameParser;
};


class EndOfGameCheck
{
public:
	EndOfGameCheck(EndOfGame::CheckType_t type, StateConditionGroup *conditions) { this->type = type; this->conditions = conditions; }
	~EndOfGameCheck();

	EndOfGame::CheckType_t type;
	StateConditionGroup *conditions;
};