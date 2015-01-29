#include <utility>
#include "Constants.h"
#include "MoveDefinition.h"
#include "Conditions.h"

MoveDefinition::MoveDefinition(char *pieceRef, Conditions *conditions, When_t when, unsigned int direction)
{
	strncpy(this->pieceRef, pieceRef, PIECE_REF_LENGTH);
	this->conditions = conditions;
	this->direction = direction;
	this->when = when;
}


MoveDefinition::~MoveDefinition()
{
	if (conditions != 0)
		delete conditions;
}


std::list<Move> Slide::AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	// todo: implement this
	std::list<Move> moves;
	return moves;
}