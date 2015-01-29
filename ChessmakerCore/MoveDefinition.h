#pragma once

#include <list>
#include "Move.h"

class Conditions;
class Move;
class MoveStep;
class Piece;

#define PIECE_REF_LENGTH 32

class MoveDefinition
{
public:
	typedef enum { Any, Moving, Capturing } When_t;

	MoveDefinition(char *pieceRef, Conditions *conditions, When_t when, unsigned int direction);
	~MoveDefinition();

	virtual std::list<Move> AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep) = 0;

private:
	char pieceRef[PIECE_REF_LENGTH];
	Conditions *conditions;
	When_t when;
	unsigned int direction;

	friend class GameParser;
};


class Slide : public MoveDefinition
{
public:
	Slide(char *pieceRef, Conditions *conditions, When_t when, unsigned int direction, int distance, int distanceMax)
		: MoveDefinition(pieceRef, conditions, when, direction)
	{
		this->distance = distance; this->distanceMax = distanceMax;
	}
	~Slide() { }

	std::list<Move> AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep);

private:
	int distance, distanceMax;

	friend class GameParser;
};