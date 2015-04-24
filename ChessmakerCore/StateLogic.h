#pragma once

#include "Definitions.h"

class StateLogicCheck;
class GameState;
class Move;
class StateConditionGroup;
class StateLogicElement;

class StateLogic
{
public:
	StateLogic(bool startOfTurn);
	~StateLogic();

	typedef enum { None, Win, Lose, Draw, IllegalMove } GameEnd_t;

	static StateLogic *CreateDefault(bool startOfTurn);

	GameEnd_t Evaluate(GameState *state, bool canMove);

private:
	bool startOfTurn;
	std::list<StateLogicElement*> elements;

	friend class GameParser;
};


class StateLogicElement
{
public:
	virtual ~StateLogicElement() { }
	virtual StateLogic::GameEnd_t Evaluate(GameState *state, bool canMove) = 0;
};


class GameEnd : public StateLogicElement
{
public:
	virtual StateLogic::GameEnd_t Evaluate(GameState* state, bool canMove);

	StateLogic::GameEnd_t type;
	// message
	// append notation
};


class StateLogicBlock : public StateLogicElement
{
public:
	StateLogicBlock(StateConditionGroup *conditions);
	virtual ~StateLogicBlock();

	virtual StateLogic::GameEnd_t Evaluate(GameState* state, bool canMove);

	StateConditionGroup *conditions;
	StateLogic *logicIfTrue, *logicIfFalse;
};
