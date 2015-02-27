#pragma once

#include <list>
#include "MoveDefinition.h"
#include "Player.h"

class Cell;
class GameState;
class Move;
class PieceType;

class MoveCondition
{
public:
	typedef enum { Equals, LessThan, GreaterThan, LessThanOrEquals, GreaterThanOrEquals } NumericComparison_t;

	virtual bool IsSatisfied(Move *move) = 0;

protected:
	bool ResolveComparison(NumericComparison_t type, int val1, int val2);
};


class MoveConditionGroup : public MoveCondition
{
public:
	typedef enum { And, Or, Nand, Nor, Xor } GroupType_t;

	MoveConditionGroup(GroupType_t type);
	~MoveConditionGroup();

	virtual bool IsSatisfied(Move *move);
private:
	GroupType_t type;
	std::list<MoveCondition*> elements;

	friend class GameParser;
};


class MoveCondition_Type : public MoveCondition
{
public:
	MoveCondition_Type(char *of, PieceType *type)
	{
		strncpy(pieceRef, of, PIECE_REF_LENGTH);
		this->type = type;
	}

	virtual bool IsSatisfied(Move *move);
private:
	char pieceRef[PIECE_REF_LENGTH];
	PieceType *type;
};


class MoveCondition_Owner : public MoveCondition
{
public:
	MoveCondition_Owner(char *of, Player::Relationship_t relationship)
	{
		strncpy(pieceRef, of, PIECE_REF_LENGTH);
		this->relationship = relationship;
	}

	virtual bool IsSatisfied(Move *move);
private:
	char pieceRef[PIECE_REF_LENGTH];
	Player::Relationship_t relationship;
};


class MoveCondition_MoveNumber : public MoveCondition
{
public:
	MoveCondition_MoveNumber(char *of, int number, NumericComparison_t comparison)
	{
		strncpy(pieceRef, of, PIECE_REF_LENGTH);
		this->number = number;
		this->comparison = comparison;
	}

	virtual bool IsSatisfied(Move *move);
private:
	char pieceRef[PIECE_REF_LENGTH];
	int number;
	MoveCondition::NumericComparison_t comparison;
};


class MoveCondition_MaxDist : public MoveCondition
{
public:
	MoveCondition_MaxDist(char *from, unsigned int dir, int number, NumericComparison_t comparison)
	{
		strncpy(pieceRef, from, PIECE_REF_LENGTH);
		this->dir = dir;
		this->number = number;
		this->comparison = comparison;
	}

	virtual bool IsSatisfied(Move *move);
private:
	char pieceRef[PIECE_REF_LENGTH];
	unsigned int dir;
	int number;
	MoveCondition::NumericComparison_t comparison;
};


class MoveCondition_TurnsSinceLastMove : public MoveCondition
{
public:
	MoveCondition_TurnsSinceLastMove(char *of, int number, NumericComparison_t comparison)
	{
		strncpy(pieceRef, of, PIECE_REF_LENGTH);
		this->number = number;
		this->comparison = comparison;
	}

	virtual bool IsSatisfied(Move *move);
private:
	char pieceRef[PIECE_REF_LENGTH];
	int number;
	MoveCondition::NumericComparison_t comparison;
};


class MoveCondition_Threatened : public MoveCondition
{
public:
	MoveCondition_Threatened(bool start, bool end, bool value)
	{
		this->start = start;
		this->end = end;
		this->value = value;
	}

	virtual bool IsSatisfied(Move *move);
private:
	bool CheckSatisfied(Move *move);
	bool IsThreatened(GameState *state, Cell *position);

	bool start, end, value;
	static bool alreadyChecking;
};