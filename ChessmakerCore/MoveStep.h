#pragma once
#include "Piece.h"

class Game;

class MoveStep
{
public:
	MoveStep(Piece *piece);
	~MoveStep();

	bool Perform(bool updateDisplay);
	bool Reverse(bool updateDisplay);

	static MoveStep *CreateMove(Piece *piece, Cell *from, Cell *to, direction_t dir, int distance);
	static MoveStep *CreateCapture(Piece *piece, Cell *from, Player *capturedBy, bool toHeld);
	static MoveStep *CreateDrop(Piece *piece, Cell *to, Player *droppedBy);
	static MoveStep *CreatePromotion(Piece *piece, PieceType *fromType, PieceType *toType);
	static MoveStep *CreateSteal(Piece *piece, Player *fromOwner, Player *toOwner);

private:
	bool Pickup(Piece::State_t state, Player *stateOwner, Cell *pos, Player *owner, PieceType *type);
	void Place(Piece::State_t state, Player *stateOwner, Cell *pos, Player *owner, PieceType *type);
	void UpdateDisplay();

	static int nextID;
	int uniqueID;

	Piece *piece;
	direction_t direction;
	int distance;
	Piece::State_t fromState, toState;
	Cell *fromPos, *toPos;
	Player *fromOwner, *toOwner;
	Player *fromStateOwner, *toStateOwner;
	PieceType *fromType, *toType;

	friend class Distance;
};

