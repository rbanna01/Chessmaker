#include "MoveStep.h"
#include "Game.h"
#include "Piece.h"

int MoveStep::nextID = 1;

MoveStep::MoveStep(Piece *piece)
{
	uniqueID = MoveStep::nextID++;
	this->piece = piece;
    fromPos = toPos = 0;
	fromOwner = toOwner = piece->GetOwner();
	fromStateOwner = toStateOwner = 0;
	fromType = toType = piece->GetType();
    direction = 0; // used only for resolving relative directions of subsequent steps
}


MoveStep::~MoveStep()
{
}


bool MoveStep::Perform(bool updateDisplay)
{
	if (!Pickup(fromState, fromStateOwner, fromPos, fromOwner, fromType))
		return false;

	Place(toState, toStateOwner, toPos, toOwner, toType, updateDisplay);

	return true;
}


bool MoveStep::Reverse(bool updateDisplay)
{
	if (!Pickup(toState, toStateOwner, toPos, toOwner, toType))
		return false;

	Place(fromState, fromStateOwner, fromPos, fromOwner, fromType, updateDisplay);

	return true;
}


bool MoveStep::Pickup(Piece::State_t state, Player *stateOwner, Cell *pos, Player *owner, PieceType *type)
{
	if (piece->GetState() != state) // piece isn't in the expected state, quit
	{
		ReportError("move error - state is wrong: got %i, expected %i for %s %s\n", piece->GetState(), state, piece->GetOwner()->GetName(), piece->GetType()->GetName());
		return false;
	}

	if (piece->GetOwner() != owner)
	{
		ReportError("move error - owner is wrong: got %s, expected %s for %s %s\n", piece->GetOwner()->GetName(), owner->GetName(), piece->GetOwner()->GetName(), piece->GetType()->GetName());
		return false;
	}

	if (piece->GetStateOwner() != stateOwner)
	{
		ReportError("move error - state owner is wrong: got %s, expected %s for %s %s\n", piece->GetStateOwner() == 0 ? "[null]" : piece->GetStateOwner()->GetName(), stateOwner == 0 ? "[null]" : stateOwner->GetName(), piece->GetOwner()->GetName(), piece->GetType()->GetName());
		return false;
	}

	switch (state)
	{
	case Piece::OnBoard:
		if (piece->GetPosition() != pos)
		{
			ReportError("move error - position is wrong: got %s, expected %s for %s %s\n", piece->GetPosition()->GetName(), pos->GetName(), piece->GetOwner()->GetName(), piece->GetType()->GetName());
			return false;
		}
		if (toState != fromState || fromOwner != toOwner)
			owner->piecesOnBoard.erase(piece);
		pos->piece = 0;
		return true;

	case Piece::Captured:
		if (stateOwner == 0 || stateOwner->piecesCaptured.erase(piece) == 0)
		{
			ReportError("move error - captured piece is not held by its state owner\n");
			return false;
		}
		return true;

	case Piece::Held:
		if (stateOwner == 0 || stateOwner->piecesHeld.erase(piece) == 0)
		{
			ReportError("move error - held piece is not held by its state owner\n");
			return false;
		}
		return true;
	default:
		ReportError("move error - piece has an invalid state: %i\n", state);
		return false;
	}
}


void MoveStep::Place(Piece::State_t state, Player *stateOwner, Cell *pos, Player *owner, PieceType *type, bool updateDisplay)
{
    piece->pieceState = state;
	piece->stateOwner = stateOwner;
	piece->position = pos;
	piece->owner = owner;
	piece->pieceType = type;
    
	switch (state) {
	case Piece::State_t::OnBoard:
		pos->piece = piece;
		if (toState != fromState || fromOwner != toOwner)
			owner->piecesOnBoard.insert(piece);
		break;
	case Piece::State_t::Captured:
		stateOwner->piecesCaptured.insert(piece);
		break;
	case Piece::State_t::Held:
		stateOwner->piecesHeld.insert(piece);
		break;
	default:
		ReportError("move error - piece has an invalid state: %i\n", state);
		return;
	}

	if (!updateDisplay)
		return;

	// move the svg element for this piece
#ifdef EMSCRIPTEN
	EM_ASM_ARGS(
		{ movePiece($0, $1, $2, $3, $4, $5, $6) },
		piece->GetID(), (int)state, stateOwner == 0 ? "" : stateOwner->GetName(), pos == 0 ? 0 : pos->GetCoordX(), pos == 0 ? 0 : pos->GetCoordY(), owner->GetName(), type->GetAppearance(owner)
	);
#endif
}


MoveStep* MoveStep::CreateMove(Piece *piece, Cell *from, Cell *to, direction_t dir, int distance)
{
	MoveStep *step = new MoveStep(piece);
	step->fromState = step->toState = Piece::State_t::OnBoard;
	step->fromPos = from;
	step->toPos = to;
	step->direction = dir;
	step->distance = distance;
	return step;
};


MoveStep* MoveStep::CreateCapture(Piece *piece, Cell *from, Player *capturedBy, bool toHeld)
{
	MoveStep *step = new MoveStep(piece);
	step->fromState = Piece::State_t::OnBoard;
	step->toState = toHeld ? Piece::State_t::Held : Piece::State_t::Captured;
	step->fromPos = from;
	step->toStateOwner = capturedBy;
	step->distance = 0;
	return step;
};


MoveStep* MoveStep::CreateDrop(Piece *piece, Cell *to, Player *droppedBy)
{
	MoveStep *step = new MoveStep(piece);
	step->fromState = Piece::State_t::Held;
	step->toState = Piece::State_t::OnBoard;
	step->fromStateOwner = droppedBy;
	step->toPos = to;
	step->distance = 0;
	return step;
};


MoveStep* MoveStep::CreatePromotion(Piece *piece, PieceType *fromType, PieceType *toType)
{
	MoveStep *step = new MoveStep(piece);
	step->fromState = step->toState = piece->GetState();
	step->fromStateOwner = step->toStateOwner = piece->GetStateOwner();
	step->fromPos = step->toPos = piece->GetPosition();
	step->fromType = fromType;
	step->toType = toType;
	step->distance = 0;
	return step;
};


MoveStep* MoveStep::CreateSteal(Piece *piece, Player *fromOwner, Player *toOwner)
{
	MoveStep *step = new MoveStep(piece);
	step->fromState = step->toState = piece->GetState();
	step->fromStateOwner = step->toStateOwner = piece->GetStateOwner();
	step->fromPos = step->toPos = piece->GetPosition();
	step->fromOwner = fromOwner;
	step->toOwner = toOwner;
	step->distance = 0;
	return step;
};