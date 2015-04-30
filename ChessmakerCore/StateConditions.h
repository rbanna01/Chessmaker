#pragma once

#include "Definitions.h"
#include "MoveConditions.h"

class StateCondition : public Condition
{
public:
	virtual bool IsSatisfied(GameState *move, bool canMove) = 0;
	virtual ~StateCondition() {}
};


class StateConditionGroup : public StateCondition
{
public:
	StateConditionGroup(GroupType_t type);
	virtual ~StateConditionGroup();

	virtual bool IsSatisfied(GameState *state, bool canMove);
private:
	GroupType_t type;
	std::list<StateCondition*> elements;

	friend class StateLogic;
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


class StateCondition_TurnsSinceLastMove : public StateCondition
{
public:
	StateCondition_TurnsSinceLastMove(PieceType *type, Player::Relationship_t owner, NumericComparison_t comparison, int number)
	{
		this->type = type;
		this->owner = owner;
		this->comparison = comparison;
		this->number = number;
	}

	virtual bool IsSatisfied(GameState *move, bool canMove);
private:
	PieceType *type;
	Player::Relationship_t owner;
	NumericComparison_t comparison;
	int number;
};


class StateCondition_TurnsSinceLastCapture : public StateCondition
{
public:
	StateCondition_TurnsSinceLastCapture(PieceType *type, Player::Relationship_t owner, NumericComparison_t comparison, int number)
	{
		this->type = type;
		this->owner = owner;
		this->comparison = comparison;
		this->number = number;
	}

	virtual bool IsSatisfied(GameState *move, bool canMove);
private:
	PieceType *type;
	Player::Relationship_t owner;
	NumericComparison_t comparison;
	int number;
};


class StateCondition_LastMoveRepetition : public StateCondition
{
public:
	StateCondition_LastMoveRepetition(int period, NumericComparison_t comparison, int number)
	{
		this->period = period;
		this->comparison = comparison;
		this->number = number;
	}

	virtual bool IsSatisfied(GameState *move, bool canMove);

private:
	NumericComparison_t comparison;
	int period, number;
};