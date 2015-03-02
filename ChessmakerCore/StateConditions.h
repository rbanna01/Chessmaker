#pragma once

#include "MoveConditions.h"

class StateCondition : public Condition
{
public:
	virtual bool IsSatisfied(GameState *move, bool canMove) = 0;
};


class StateConditionGroup : public StateCondition
{
public:
	StateConditionGroup(GroupType_t type);
	~StateConditionGroup();

	virtual bool IsSatisfied(GameState *state, bool canMove);
private:
	GroupType_t type;
	std::list<StateCondition*> elements;

	friend class EndOfGame;
	friend class GameParser;
};


class StateCondition_CannotMove : public StateCondition
{
public:
	StateCondition_CannotMove() { }

	virtual bool IsSatisfied(GameState *move, bool canMove);
};


class StateCondition_Threatened : public StateCondition
{
public:
	StateCondition_Threatened(PieceType *type)
	{
		this->type = type;
	}

	virtual bool IsSatisfied(GameState *move, bool canMove);
private:
	PieceType *type;
};