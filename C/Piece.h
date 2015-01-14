#pragma once
#include "Cell.h"
#include "PieceType.h"
#include "Player.h"

typedef enum { OnBoard, Captured, Held } PieceState_t;

class Piece
{
public:
	Piece(Player *owner, PieceType *type, Cell *position, PieceState_t state, Player *stateOwner);
	~Piece();

	bool CanCapture(Piece *target);
	bool TypeMatches(/*std::string type*/ PieceType *type);

	Player *GetOwner() { return owner; }
	Player *GetStateOwner() { return stateOwner; }
	PieceType *GetType() { return pieceType; }
	Cell *GetPosition() { return position; }
	PieceState_t GetState() { return pieceState; }
private:
	static int nextID;
	int uniqueID, moveNumber, lastMoveTurn;
	Player *owner;
	Player *stateOwner;
	PieceType *pieceType;
	Cell *position;
	PieceState_t pieceState;

	friend class Move;
	friend class MoveStep;
};

