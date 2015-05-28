#pragma once

#include "Definitions.h"
#include "Cell.h"
#include "PieceType.h"
#include "Player.h"

class Piece
{
public:
	typedef enum { OnBoard, Captured, Held } State_t;

	Piece(Player *owner, PieceType *type, Cell *position, State_t state);
	~Piece();

	bool CanCapture(Piece *target);
	bool TypeMatches(/*std::string type*/ PieceType *type);

	Player *GetOwner() { return owner; }
	PieceType *GetType() { return pieceType; }
	Cell *GetPosition() { return position; }
	State_t GetState() { return pieceState; }
	int GetMoveNumber() { return moveNumber; }
	int GetLastMoveTurn() { return lastMoveTurn; }
	bool HasState(customstate_t state) { return (customState & state) != 0; }
	int GetID() { return uniqueID; }
private:
	static int nextID;
	int uniqueID, moveNumber, lastMoveTurn;
	Player *owner;
	PieceType *pieceType;
	Cell *position;
	State_t pieceState;
	customstate_t customState;

	friend class Move;
	friend class MoveStep;
	friend class GameParser;
};

