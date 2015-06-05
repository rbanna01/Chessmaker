#pragma once

#include "Definitions.h"
#include "Piece.h"

class Game;

class MoveStep
{
public:
	MoveStep(Piece *piece);
	~MoveStep();

	bool Perform(bool updateDisplay);
	bool Reverse(bool updateDisplay);

	Piece *GetPiece() { return piece; }
	direction_t GetDirection() { return direction; }
	Piece::State_t GetFromState() { return fromState; }
	Piece::State_t GetToState() { return toState; }
	Cell *GetFromPosition() { return fromPos; }
	Cell *GetToPosition() { return toPos; }
	int GetNumber() { return number; }

	static MoveStep *CreateMove(Piece *piece, Cell *from, Cell *to, direction_t dir, int distance);
	static MoveStep *CreateCapture(Piece *piece, Cell *from);
	static MoveStep *CreatePickup(Piece *piece, Cell *from, Player *heldBy);
	static MoveStep *CreateDrop(Piece *piece, Cell *to, Player *droppedBy);
	static MoveStep *CreatePromotion(Piece *piece, PieceType *fromType, PieceType *toType);
	static MoveStep *CreateSteal(Piece *piece, Player *fromOwner, Player *toOwner);
	static MoveStep *CreateAddState(Piece *piece, customstate_t state);
	static MoveStep *CreateRemoveState(Piece *piece, customstate_t state);

private:
	bool Pickup(Piece::State_t state, Cell *pos, Player *owner, PieceType *type);
	bool Place(Piece::State_t state, Cell *pos, Player *owner, PieceType *type, customstate_t addState, customstate_t removeState, bool updateDisplay);
	void UpdateDisplay();

	int number;

	Piece *piece;
	direction_t direction;
	int distance;
	Piece::State_t fromState, toState;
	Cell *fromPos, *toPos;
	Player *fromOwner, *toOwner;
	PieceType *fromType, *toType;
	customstate_t addState, removeState;

	friend class Distance;
	friend class Move;
};

