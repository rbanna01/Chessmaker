#pragma once

#include <list>

class StateConditions;
class EndOfGameCheck;


class EndOfGame
{
public:
	EndOfGame();
	~EndOfGame();

	static EndOfGame *CreateDefault();

	typedef enum { Win, Lose, Draw, IllegalMove } CheckType_t;

private:
	bool illegalMovesSpecified;

	std::list<EndOfGameCheck*> startOfTurnChecks, endOfTurnChecks;

	friend class GameParser;
};


class EndOfGameCheck
{
public:
	EndOfGameCheck(EndOfGame::CheckType_t type, StateConditions *conditions) { this->type = type; this->conditions = conditions; }
	~EndOfGameCheck() { delete conditions; }

	EndOfGame::CheckType_t type;
	StateConditions *conditions;
};