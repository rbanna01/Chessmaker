#include <utility>
#include "Constants.h"
#include "MoveDefinition.h"
#include "MoveConditions.h"

MoveDefinition::MoveDefinition(char *pieceRef, MoveConditions *conditions, When_t when, unsigned int direction)
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


std::list<Move> Leap::AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	// todo: implement this
	std::list<Move> moves;
	return moves;
}


std::list<Move> Hop::AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	// todo: implement this
	std::list<Move> moves;
	return moves;
}


std::list<Move> Shoot::AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	// todo: implement this
	std::list<Move> moves;
	return moves;
}


std::list<Move> MoveLike::AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	// todo: implement this
	std::list<Move> moves;
	return moves;
}


std::list<Move> ReferencePiece::AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	// todo: implement this
	std::list<Move> moves;
	return moves;
}


std::list<Move> MoveGroup::AppendValidNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	// todo: implement this
	std::list<Move> moves;
	return moves;
}