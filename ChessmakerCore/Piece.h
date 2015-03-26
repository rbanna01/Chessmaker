#pragma once

#include "Definitions.h"
#include "Cell.h"
#include "PieceType.h"
#include "Player.h"

class Piece
{
public:
	typedef enum { OnBoard, Captured, Held } State_t;

	Piece(Player *owner, PieceType *type, Cell *position, State_t state, Player *stateOwner);
	~Piece();

	bool CanCapture(Piece *target);
	bool TypeMatches(/*std::string type*/ PieceType *type);

	Player *GetOwner() { return owner; }
	Player *GetStateOwner() { return stateOwner; }
	PieceType *GetType() { return pieceType; }
	Cell *GetPosition() { return position; }
	State_t GetState() { return pieceState; }

	int GetID() { return uniqueID; }
private:
	static int nextID;
	int uniqueID, moveNumber, lastMoveTurn;
	Player *owner;
	Player *stateOwner;
	PieceType *pieceType;
	Cell *position;
	State_t pieceState;

	friend class Move;
	friend class MoveStep;
	friend class GameParser;
};

