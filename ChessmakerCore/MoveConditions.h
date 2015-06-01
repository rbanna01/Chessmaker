#pragma once

#include "Definitions.h"
#include "MoveDefinition.h"
#include "Player.h"

class Cell;
class GameState;
class Move;
class PieceType;

class Condition
{
public:
	typedef enum { Equals, LessThan, GreaterThan, LessThanOrEquals, GreaterThanOrEquals } NumericComparison_t;
	typedef enum { And, Or, Nand, Nor, Xor } GroupType_t;

protected:
	bool ResolveComparison(NumericComparison_t type, int val1, int val2);
};


class MoveCondition : public Condition
{
public:
	virtual ~MoveCondition() {}
	virtual bool IsSatisfied(Move *move, MoveStep *lastPerformed) = 0;
};


class MoveConditionGroup : public MoveCondition
{
public:
	MoveConditionGroup(GroupType_t type);
	virtual ~MoveConditionGroup();

	virtual bool IsSatisfied(Move *move, MoveStep *lastPerformed);
private:
	GroupType_t type;
	std::list<MoveCondition*> elements;

	friend class GameParser;
};


class MoveCondition_Type : public MoveCondition
{
public:
	MoveCondition_Type(const char *of)
	{
		strcpy(pieceRef, of);
		type = 0;
	}

	virtual bool IsSatisfied(Move *move, MoveStep *lastPerformed);
private:
	char pieceRef[PIECE_REF_LENGTH];
	PieceType *type;

	friend class GameParser;
};


class MoveCondition_Owner : public MoveCondition
{
public:
	MoveCondition_Owner(const char *of, Player::Relationship_t relationship)
	{
		strcpy(pieceRef, of);
		this->relationship = relationship;
	}

	virtual bool IsSatisfied(Move *move, MoveStep *lastPerformed);
private:
	char pieceRef[PIECE_REF_LENGTH];
	Player::Relationship_t relationship;
};


class MoveCondition_MoveNumber : public MoveCondition
{
public:
	MoveCondition_MoveNumber(const char *of, int number, NumericComparison_t comparison)
	{
		strcpy(pieceRef, of);
		this->number = number;
		this->comparison = comparison;
	}

	virtual bool IsSatisfied(Move *move, MoveStep *lastPerformed);
private:
	char pieceRef[PIECE_REF_LENGTH];
	int number;
	MoveCondition::NumericComparison_t comparison;
};


class MoveCondition_MaxDist : public MoveCondition
{
public:
	MoveCondition_MaxDist(const char *from, direction_t dir, int number, NumericComparison_t comparison)
	{
		strcpy(pieceRef, from);
		this->dir = dir;
		this->number = number;
		this->comparison = comparison;
	}

	virtual bool IsSatisfied(Move *move, MoveStep *lastPerformed);
private:
	char pieceRef[PIECE_REF_LENGTH];
	direction_t dir;
	int number;
	MoveCondition::NumericComparison_t comparison;
};


class MoveCondition_TurnsSinceLastMove : public MoveCondition
{
public:
	MoveCondition_TurnsSinceLastMove(const char *of, int number, NumericComparison_t comparison)
	{
		strcpy(pieceRef, of);
		this->number = number;
		this->comparison = comparison;
	}

	virtual bool IsSatisfied(Move *move, MoveStep *lastPerformed);
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

	virtual bool IsSatisfied(Move *move, MoveStep *lastPerformed);
private:
	bool CheckSatisfied(Move *move, MoveStep *lastPerformed);
	static bool IsThreatened(GameState *state, Cell *position);

	bool start, end, value;
	static bool checkingThreat;

	friend class GameState;
	friend class StateCondition_Threatened;
};


class MoveCondition_Count : public MoveCondition
{
public:
	MoveCondition_Count(const char *from, direction_t dir, Distance *dist, Player::Relationship_t relationship, NumericComparison_t comparison, int number)
	{
		strcpy(pieceRef, from);
		this->dir = dir;
		this->distance = dist;
		this->relationship = relationship;
		this->number = number;
		this->comparison = comparison;
		type = 0;
	}

	~MoveCondition_Count() { if (distance != &Distance::Any) delete distance; }

	virtual bool IsSatisfied(Move *move, MoveStep *lastPerformed);
	int GetCount(Move *move, MoveStep *lastPerformed);
private:
	char pieceRef[PIECE_REF_LENGTH];
	direction_t dir;
	Distance *distance;
	PieceType *type;
	Player::Relationship_t relationship;
	int number;
	MoveCondition::NumericComparison_t comparison;

	friend class GameParser;
};


class MoveCondition_CountMultiple : public MoveCondition
{
public:
	typedef enum { Add, Subtract } Operation_t;

	MoveCondition_CountMultiple(Operation_t operation, NumericComparison_t comparison, int number)
	{
		this->operation = operation;
		this->number = number;
		this->comparison = comparison;
	}

	~MoveCondition_CountMultiple()
	{
		for (auto it = items.begin(); it != items.end(); it++)
			delete *it;
	}

	void AddItem(MoveCondition_Count* item) { items.push_back(item); }
	virtual bool IsSatisfied(Move *move, MoveStep *lastPerformed);
private:
	std::list<MoveCondition_Count*> items;
	Operation_t operation;
	int number;
	MoveCondition::NumericComparison_t comparison;
};


class MoveCondition_State : public MoveCondition
{
public:
	MoveCondition_State(const char *piece, customstate_t state)
	{
		strcpy(pieceRef, piece);
		this->state = state;
	}

	virtual bool IsSatisfied(Move *move, MoveStep *lastPerformed);
private:
	char pieceRef[PIECE_REF_LENGTH];
	customstate_t state;

	friend class GameState;
	friend class StateCondition_Threatened;
};


class MoveCondition_ReferencePiece : public MoveCondition
{
public:
	MoveCondition_ReferencePiece(const char *pieceRef, Player::Relationship_t relationship, direction_t dir, Distance *distance)
	{
		strcpy(this->otherPieceRef, pieceRef);
		this->relationship = relationship;
		this->dir = dir;
		this->distance = distance;
		otherPieceType = 0;
	}
	virtual ~MoveCondition_ReferencePiece() { if (distance != 0 && distance != &Distance::Any) delete distance; }

	virtual bool IsSatisfied(Move *move, MoveStep *lastPerformed);
private:
	char otherPieceRef[PIECE_REF_LENGTH];
	PieceType *otherPieceType;
	Player::Relationship_t relationship;
	direction_t dir;
	Distance *distance;

	friend class GameParser;
};