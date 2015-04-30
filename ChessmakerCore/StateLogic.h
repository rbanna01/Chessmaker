#pragma once

#include "Definitions.h"

class StateLogicCheck;
class GameEnd;
class GameState;
class Move;
class StateConditionGroup;
class StateLogicElement;

class StateLogic
{
public:
	StateLogic(bool startOfTurn, bool root);
	~StateLogic();

	typedef enum { None, Win, Lose, Draw, IllegalMove } GameEnd_t;

	GameEnd* Evaluate(GameState *state, bool canMove);

private:
	bool startOfTurn, root;
	std::list<StateLogicElement*> elements;

	friend class GameParser;
};

#define LOGIC_MESSAGE_LENGTH 101
#define APPEND_NOTATION_LENGTH 6

class StateLogicElement
{
public:
	virtual ~StateLogicElement() { }
	virtual GameEnd *Evaluate(GameState *state, bool canMove) = 0;
};


class GameEnd : public StateLogicElement
{
public:
	GameEnd(StateLogic::GameEnd_t type, const char *message, const char *appendNotation = "");
	virtual GameEnd *Evaluate(GameState* state, bool canMove) { return this; }
	StateLogic::GameEnd_t GetType() { return type; }
	const char *GetMessage() { return message; }
	const char *GetAppendNotation() { return appendNotation; }

private:
	StateLogic::GameEnd_t type;
	char message[LOGIC_MESSAGE_LENGTH];
	char appendNotation[APPEND_NOTATION_LENGTH];

	friend class GameParser;
};


class StateLogicBlock : public StateLogicElement
{
public:
	StateLogicBlock(StateConditionGroup *conditions);
	virtual ~StateLogicBlock();

	virtual GameEnd *Evaluate(GameState* state, bool canMove);

	StateConditionGroup *conditions;
	StateLogic *logicIfTrue, *logicIfFalse;
};
