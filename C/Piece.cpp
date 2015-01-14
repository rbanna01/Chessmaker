#include "Piece.h"

int Piece::nextID = 1;

Piece::Piece(Player *owner, PieceType *type, Cell *position, PieceState_t state, Player *stateOwner)
{
	uniqueID = Piece::nextID++; // prefix this with a "p" in the DOM, i guess
	owner = owner;
	position = position;
	pieceType = type;
	pieceState = state;
	stateOwner = stateOwner;
	moveNumber = 1;		// how is this set, when loaded?
	lastMoveTurn = 0;	// how is this set, when loaded?
}


Piece::~Piece()
{
}


bool Piece::CanCapture(Piece *target)
{
	return owner->GetRelationship(target->owner) == Enemy;
}

bool Piece::TypeMatches(PieceType *type)
{
	if (type == 0) // "any"
		return true;
	return type == pieceType;
}