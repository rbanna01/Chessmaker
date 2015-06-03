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

	MoveDefinition(const char *pieceRef, MoveConditionGroup *conditions, When_t when, direction_t direction);
	virtual ~MoveDefinition();

	void SetCustomNotation(std::string notation) { this->notation = notation; }
	std::string GetCustomNotation() { return this->notation; }

	virtual std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep) = 0;
protected:
	MoveStep *CreateCapture(Piece *target, Piece *capturedBy);

	char pieceRef[PIECE_REF_LENGTH];
	MoveConditionGroup *conditions;
	When_t when;
	direction_t direction;
	bool moveSelf;

private:
	std::string notation;
	friend class GameParser;
};


class Slide : public MoveDefinition
{
public:
	Slide(const char *pieceRef, MoveConditionGroup *conditions, When_t when, direction_t direction, Distance *distance, Distance *distanceMax)
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
	Leap(const char *pieceRef, MoveConditionGroup *conditions, When_t when, direction_t direction, Distance *distance, Distance *distanceMax, direction_t secondDir, Distance *secondDist)
		: MoveDefinition(pieceRef, conditions, when, direction)
	{
		this->distance = distance; this->distanceMax = distanceMax;
		this->secondDir = secondDir; this->secondDist = secondDist;
	}
	virtual ~Leap() { if (distance != &Distance::Any) delete distance; if (distanceMax != 0 && distanceMax != &Distance::Any) delete distanceMax; if (secondDist != &Distance::Zero && secondDist != &Distance::Any) delete secondDist; }

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	Distance *distance, *distanceMax;
	direction_t secondDir;
	Distance *secondDist;

	friend class GameParser;
};


class Hop : public MoveDefinition
{
public:
	Hop(const char *pieceRef, MoveConditionGroup *conditions, When_t when, direction_t direction, Distance *distToHurdle, Distance *distToHurdleMax, Distance *distAfterHurdle, Distance *distAfterHurdleMax, bool captureHurdle)
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
	Shoot(const char *pieceRef, MoveConditionGroup *conditions, When_t when, direction_t direction, Distance *distance, Distance *distanceMax, direction_t secondDir, Distance *secondDist)
		: MoveDefinition(pieceRef, conditions, when, direction)
	{
		this->distance = distance; this->distanceMax = distanceMax;
		this->secondDir = secondDir; this->secondDist = secondDist;
	}
	virtual ~Shoot() { if (distance != &Distance::Any) delete distance; if (distanceMax != 0 && distanceMax != &Distance::Any) delete distanceMax; if (secondDist != &Distance::Zero && secondDist != &Distance::Any) delete secondDist; }

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	Distance *distance, *distanceMax;
	direction_t secondDir;
	Distance *secondDist;

	friend class GameParser;
};


class MoveLike : public MoveDefinition
{
public:
	MoveLike(const char *pieceRef, MoveConditionGroup *conditions, When_t when)
		: MoveDefinition("", conditions, when, 0)
	{
		likeTarget = strcmp(pieceRef, "target") == 0;
	}
	virtual ~MoveLike() { }

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	std::list<Move*> *AppendMoveLikeTarget(Move *baseMove, Piece *piece, MoveStep *previousStep);

	static bool AllowMoveLikeTarget;
	bool likeTarget;

	friend class GameParser;
};


class ReferencePiece : public MoveDefinition
{
public:
	ReferencePiece(const char *pieceRef, Player::Relationship_t relationship, direction_t dir, Distance *distance)
		: MoveDefinition("", 0, Any, dir)
	{
		strcpy(this->otherPieceRef, pieceRef);
		this->relationship = relationship;
		this->distance = distance;
		otherPieceType = 0;
	}
	virtual ~ReferencePiece() { if (distance != 0 && distance != &Distance::Any) delete distance; }

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);
	static Piece *FindReferencedPiece(Piece *piece, Move *move, MoveStep *previousStep, Player::Relationship_t relationship, PieceType *type, Distance *distance, direction_t direction);
private:
	char otherPieceRef[PIECE_REF_LENGTH];
	PieceType *otherPieceType;
	Player::Relationship_t relationship;
	Distance *distance;

	friend class GameParser;
};


class SetState : public MoveDefinition
{
public:
	typedef enum { Set, Clear, SetAndClear } Mode_t;

	SetState(const char *pieceRef, customstate_t state, Mode_t mode, MoveConditionGroup *conditions)
		: MoveDefinition(pieceRef, conditions, Any, 0)
	{
		this->state = state;
		this->mode = mode;
	}
	virtual ~SetState() { }

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	customstate_t state;
	Mode_t mode;

	friend class GameParser;
};


class Promotion : public MoveDefinition
{
public:
	Promotion(const char *pieceRef, MoveConditionGroup *conditions, bool appendNotation)
		: MoveDefinition(pieceRef, conditions, Any, 0)
	{
		this->appendNotation = appendNotation;
	}

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	std::list<PieceType*> options;
	bool appendNotation;

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
	std::list<Move*> *DetermineChildSteps(Piece *piece, Move *baseMove, std::list<MoveDefinition*>::iterator it, MoveStep *previousStep, int repeatNum, std::list<Move*> *prevRepeatMoves);

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


class ForEachPiece : public MoveDefinition
{
public:
	ForEachPiece(Player::Relationship_t owner, Sequence *move)
		: MoveDefinition("", 0, Any, 0)
	{
		whenPossible = move;
		this->relationship = owner;
	}
	virtual ~ForEachPiece() { delete whenPossible; }

	std::list<Move*> *DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);
private:
	Sequence *whenPossible;
	Player::Relationship_t relationship;
};