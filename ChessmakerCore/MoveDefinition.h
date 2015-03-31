#pragma once

#include "Definitions.h"
#include "Distance.h"
#include "Move.h"
#include "PieceType.h"
#include "Player.h"

class MoveConditionGroup;
class MoveStep;
class Piece;

#define PIECE_REF_LENGTH 32

class MoveDefinition
{
public:
	typedef enum { Any, Moving, Capturing } When_t;

	MoveDefinition(char *pieceRef, MoveConditionGroup *conditions, When_t when, unsigned int direction);
	virtual ~MoveDefinition();

	virtual std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep) = 0;
protected:
	char pieceRef[PIECE_REF_LENGTH];
	MoveConditionGroup *conditions;
	When_t when;
	unsigned int direction;
	bool moveSelf;

	friend class GameParser;
};


class Slide : public MoveDefinition
{
public:
	Slide(char *pieceRef, MoveConditionGroup *conditions, When_t when, unsigned int direction, Distance *distance, Distance *distanceMax)
		: MoveDefinition(pieceRef, conditions, when, direction)
	{
		this->distance = distance; this->distanceMax = distanceMax;
	}
	virtual ~Slide() { if (distance != &Distance::Any) delete distance; if (distanceMax != 0) delete distanceMax; }

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	Distance *distance;
	Distance *distanceMax;

	friend class GameParser;
};


class Leap : public MoveDefinition
{
public:
	Leap(char *pieceRef, MoveConditionGroup *conditions, When_t when, unsigned int direction, Distance *distance, Distance *distanceMax, unsigned int secondDir, Distance *secondDist)
		: MoveDefinition(pieceRef, conditions, when, direction)
	{
		this->distance = distance; this->distanceMax = distanceMax;
		this->secondDir = secondDir; this->secondDist = secondDist;
	}
	virtual ~Leap() { if (distance != &Distance::Any) delete distance; if (distanceMax != 0) delete distanceMax; if (secondDist != &Distance::Any) delete secondDist; }

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	Distance *distance, *distanceMax;
	unsigned int secondDir;
	Distance *secondDist;

	friend class GameParser;
};


class Hop : public MoveDefinition
{
public:
	Hop(char *pieceRef, MoveConditionGroup *conditions, When_t when, unsigned int direction, Distance *distToHurdle, Distance *distToHurdleMax, Distance *distAfterHurdle, Distance *distAfterHurdleMax, bool captureHurdle)
		: MoveDefinition(pieceRef, conditions, when, direction)
	{
		this->distToHurdle = distToHurdle; this->distToHurdleMax = distToHurdleMax;
		this->distAfterHurdle = distAfterHurdle; this->distAfterHurdleMax = distAfterHurdleMax;
		this->captureHurdle = captureHurdle;
	}
	virtual ~Hop() { if (distToHurdle != &Distance::Any) delete distToHurdle; if (distToHurdleMax != 0) delete distToHurdleMax; delete distAfterHurdle; if (distAfterHurdleMax != 0 && distAfterHurdleMax != &Distance::Any) delete distAfterHurdleMax; }

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	Distance *distToHurdle, *distToHurdleMax;
	Distance *distAfterHurdle, *distAfterHurdleMax;
	bool captureHurdle;

	friend class GameParser;
};


class Shoot : public MoveDefinition
{
public:
	Shoot(char *pieceRef, MoveConditionGroup *conditions, When_t when, unsigned int direction, Distance *distance, Distance *distanceMax, unsigned int secondDir, Distance *secondDist)
		: MoveDefinition(pieceRef, conditions, when, direction)
	{
		this->distance = distance; this->distanceMax = distanceMax;
		this->secondDir = secondDir; this->secondDist = secondDist;
	}
	virtual ~Shoot() { if (distance != &Distance::Any) delete distance; if (distanceMax != 0) delete distanceMax; if (secondDist != 0 && secondDist != &Distance::Any) delete secondDist; }

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	Distance *distance, *distanceMax;
	unsigned int secondDir;
	Distance *secondDist;

	friend class GameParser;
};


class MoveLike : public MoveDefinition
{
public:
	MoveLike(char *pieceRef, MoveConditionGroup *conditions, When_t when)
		: MoveDefinition("", conditions, when, 0)
	{
		strcpy(this->otherPieceRef, pieceRef);
		likeTarget = strcmp(pieceRef, "target") == 0;
	}
	virtual ~MoveLike() { }

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	std::list<Move*> *AppendMoveLikeTarget(Move *baseMove, Piece *piece, MoveStep *previousStep);

	static bool AllowMoveLikeTarget;
	bool likeTarget;
	char otherPieceRef[PIECE_REF_LENGTH];

	friend class GameParser;
};


class ReferencePiece : public MoveDefinition
{
public:
	ReferencePiece(char *pieceRef, PieceType *type, Player::Relationship_t relationship, unsigned int dir, Distance *distance)
		: MoveDefinition("", 0, Any, 0)
	{
		strcpy(this->otherPieceRef, pieceRef);
		this->relationship = relationship;
		this->distance = distance;
		this->otherPieceDirection = dir;
	}
	virtual ~ReferencePiece() { if (distance != 0 && distance != &Distance::Any) delete distance; }

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	char otherPieceRef[PIECE_REF_LENGTH];
	PieceType *otherPieceType;
	Player::Relationship_t relationship;
	Distance *distance;
	unsigned int otherPieceDirection;

	friend class GameParser;
};


class MoveGroup : public MoveDefinition
{
public:
	MoveGroup(int minOccurs, int maxOccurs, bool stepOutIfFail)
		: MoveDefinition("", 0, Any, 0)
	{
		this->minOccurs = minOccurs;
		this->maxOccurs = maxOccurs;
		this->stepOutIfFail = stepOutIfFail;
	}
	virtual ~MoveGroup()
	{
		while (!contents.empty())
			delete contents.front(), contents.pop_front();
	}

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	int minOccurs, maxOccurs;
	bool stepOutIfFail;

	std::list<MoveDefinition*> contents;

	friend class GameParser;
};


class Sequence : public MoveGroup
{
public:
	Sequence()
		: MoveGroup(1, 1, false)
	{ }
	virtual ~Sequence() { }

private:
	friend class GameParser;
};


class Repeat : public MoveGroup
{
public:
	Repeat(int minOccurs, int maxOccurs)
		: MoveGroup(minOccurs, maxOccurs, false)
	{ }
	virtual ~Repeat() { }

private:
	friend class GameParser;
};


class WhenPossible : public MoveGroup
{
public:
	WhenPossible()
		: MoveGroup(1, 1, true)
	{ }
	virtual ~WhenPossible() { }

private:
	friend class GameParser;
};