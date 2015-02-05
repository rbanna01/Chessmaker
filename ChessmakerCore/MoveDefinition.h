#pragma once

#include <list>
#include "Distance.h"
#include "Move.h"
#include "PieceType.h"
#include "Player.h"

class MoveConditions;
class MoveStep;
class Piece;

#define PIECE_REF_LENGTH 32

class MoveDefinition
{
public:
	typedef enum { Any, Moving, Capturing } When_t;

	MoveDefinition(char *pieceRef, MoveConditions *conditions, When_t when, unsigned int direction);
	~MoveDefinition();

	virtual std::list<Move> AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep) = 0;

protected:
	char pieceRef[PIECE_REF_LENGTH];
	MoveConditions *conditions;
	When_t when;
	unsigned int direction;

	friend class GameParser;
};


class Slide : public MoveDefinition
{
public:
	Slide(char *pieceRef, MoveConditions *conditions, When_t when, unsigned int direction, Distance *distance, Distance *distanceMax)
		: MoveDefinition(pieceRef, conditions, when, direction)
	{
		this->distance = distance; this->distanceMax = distanceMax;
	}
	~Slide() { delete distance; if (distanceMax != 0) delete distanceMax; }

	std::list<Move> AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	Distance *distance;
	Distance *distanceMax;

	friend class GameParser;
};


class Leap : public MoveDefinition
{
public:
	Leap(char *pieceRef, MoveConditions *conditions, When_t when, unsigned int direction, Distance *distance, Distance *distanceMax, unsigned int secondDir, Distance *secondDist)
		: MoveDefinition(pieceRef, conditions, when, direction)
	{
		this->distance = distance; this->distanceMax = distanceMax;
		this->secondDir = secondDir; this->secondDist = secondDist;
	}
	~Leap() { delete distance; if (distanceMax != 0) delete distanceMax; if (secondDist != 0) delete secondDist; }

	std::list<Move> AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	Distance *distance, *distanceMax;
	unsigned int secondDir;
	Distance *secondDist;

	friend class GameParser;
};


class Hop : public MoveDefinition
{
public:
	Hop(char *pieceRef, MoveConditions *conditions, When_t when, unsigned int direction, Distance *distToHurdle, Distance *distToHurdleMax, Distance *distAfterHurdle, Distance *distAfterHurdleMax, bool captureHurdle)
		: MoveDefinition(pieceRef, conditions, when, direction)
	{
		this->distToHurdle = distToHurdle; this->distToHurdleMax = distToHurdleMax;
		this->distAfterHurdle = distAfterHurdle; this->distAfterHurdleMax = distAfterHurdleMax;
		this->captureHurdle = captureHurdle;
	}
	~Hop() { delete distToHurdle; if (distToHurdleMax != 0) delete distToHurdleMax; delete distAfterHurdle; if (distAfterHurdleMax != 0) delete distAfterHurdleMax; }

	std::list<Move> AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	Distance *distToHurdle, *distToHurdleMax;
	Distance *distAfterHurdle, *distAfterHurdleMax;
	bool captureHurdle;

	friend class GameParser;
};


class Shoot : public MoveDefinition
{
public:
	Shoot(char *pieceRef, MoveConditions *conditions, When_t when, unsigned int direction, Distance *distance, Distance *distanceMax, unsigned int secondDir, Distance *secondDist)
		: MoveDefinition(pieceRef, conditions, when, direction)
	{
		this->distance = distance; this->distanceMax = distanceMax;
		this->secondDir = secondDir; this->secondDist = secondDist;
	}
	~Shoot() { delete distance; if (distanceMax != 0) delete distanceMax; if (secondDist != 0) delete secondDist; }

	std::list<Move> AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	Distance *distance, *distanceMax;
	unsigned int secondDir;
	Distance *secondDist;

	friend class GameParser;
};


class MoveLike : public MoveDefinition
{
public:
	MoveLike(char *pieceRef, MoveConditions *conditions, When_t when)
		: MoveDefinition("", conditions, when, 0)
	{
		strncpy(this->otherPieceRef, pieceRef, PIECE_REF_LENGTH);
	}
	~MoveLike() { }

	std::list<Move> AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	char otherPieceRef[PIECE_REF_LENGTH];

	friend class GameParser;
};


class ReferencePiece : public MoveDefinition
{
public:
	ReferencePiece(char *pieceRef, char *type, Player::Relationship_t relationship, unsigned int dir, Distance *distance)
		: MoveDefinition("", 0, Any, 0)
	{
		strncpy(this->otherPieceRef, pieceRef, PIECE_REF_LENGTH);
		strncpy(this->otherPieceType, type, TYPE_NAME_LENGTH);
		this->relationship = relationship;
		this->distance = distance;
		this->otherPieceDirection = dir;
	}
	~ReferencePiece() { if (distance != 0) delete distance; }

	std::list<Move> AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	char otherPieceRef[PIECE_REF_LENGTH];
	char otherPieceType[TYPE_NAME_LENGTH];
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
	~MoveGroup()
	{
		std::list<MoveDefinition*>::iterator it = contents.begin();
		while (it != contents.end())
		{
			delete (*it);
			it++;
		}
		contents.clear();
	}

	std::list<Move> AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

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
	~Sequence() { }

private:
	friend class GameParser;
};


class Repeat : public MoveGroup
{
public:
	Repeat(int minOccurs, int maxOccurs)
		: MoveGroup(minOccurs, maxOccurs, false)
	{ }
	~Repeat() { }

private:
	friend class GameParser;
};


class WhenPossible : public MoveGroup
{
public:
	WhenPossible()
		: MoveGroup(1, 1, true)
	{ }
	~WhenPossible() { }

private:
	friend class GameParser;
};