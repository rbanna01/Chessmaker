#pragma once

#include "Definitions.h"
#include "MoveConditions.h"

class StateCondition : public Condition
{
public:
	virtual bool IsSatisfied(GameState *state, bool canMove) = 0;
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

	virtual bool IsSatisfied(GameState *state, bool canMove);
};


class StateCondition_Threatened : public StateCondition
{
public:
	StateCondition_Threatened(PieceType *type)
	{
		this->type = type;
	}

	virtual bool IsSatisfied(GameState *state, bool canMove);
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

	virtual bool IsSatisfied(GameState *state, bool canMove);
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

	virtual bool IsSatisfied(GameState *state, bool canMove);
private:
	PieceType *type;
	Player::Relationship_t owner;
	NumericComparison_t comparison;
	int number;
};


class StateCondition_RepetitionOfPosition : public StateCondition
{
public:
	StateCondition_RepetitionOfPosition(NumericComparison_t comparison, int number)
	{
		this->comparison = comparison;
		this->number = number;
		if (positionRepetitions == 0)
			positionRepetitions = new std::map<const char*, int, char_cmp>();
	}
	virtual ~StateCondition_RepetitionOfPosition() { if (positionRepetitions != 0) { delete positionRepetitions; positionRepetitions = 0; } }

	virtual bool IsSatisfied(GameState *state, bool canMove);
	static void DecrementCount(GameState *state);

private:
	static int IncrementCount(GameState *state);
	static std::map<const char*, int, char_cmp> *positionRepetitions;

	NumericComparison_t comparison;
	int number;
};