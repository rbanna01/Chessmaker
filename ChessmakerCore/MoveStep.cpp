#include "MoveStep.h"
#include "Game.h"
#include "Piece.h"

MoveStep::MoveStep(Piece *piece)
{
	this->piece = piece;
    fromPos = toPos = 0;
	fromOwner = toOwner = piece->GetOwner();
	fromType = toType = piece->GetType();
    direction = 0; // used only for resolving relative directions of subsequent steps
	addState = removeState = 0;
}


MoveStep::~MoveStep()
{
}


bool MoveStep::Perform(bool updateDisplay)
{
	if (!Pickup(fromState, fromPos, fromOwner, fromType))
		return false;

	return Place(toState, toPos, toOwner, toType, updateDisplay);
}


bool MoveStep::Reverse(bool updateDisplay)
{
	if (!Pickup(toState, toPos, toOwner, toType))
		return false;

	return Place(fromState, fromPos, fromOwner, fromType, updateDisplay);
}


bool MoveStep::Pickup(Piece::State_t state, Cell *pos, Player *owner, PieceType *type)
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
		if (owner->piecesCaptured.erase(piece) == 0)
		{
			ReportError("move error - captured piece is not held by its owner\n");
			return false;
		}
		return true;

	case Piece::Held:
		if (owner->piecesHeld.erase(piece) == 0)
		{
			ReportError("move error - held piece is not held by its owner\n");
			return false;
		}
		return true;
	default:
		ReportError("move error - piece has an invalid state: %i\n", state);
		return false;
	}
}


bool MoveStep::Place(Piece::State_t state, Cell *pos, Player *owner, PieceType *type, bool updateDisplay)
{
	switch (state) {
	case Piece::State_t::OnBoard:
		if (pos->piece != 0 && pos->piece != piece)
		{
			ReportError("move error - destination cell already has another piece in it: %s %s (moving %s %s)\n", pos->piece->GetOwner()->GetName(), pos->piece->GetType()->GetName(), piece->GetOwner()->GetName(), piece->GetType()->GetName());
			return false;
		}
		pos->piece = piece;
		if (toState != fromState || fromOwner != toOwner)
			owner->piecesOnBoard.insert(piece);
		break;
	case Piece::State_t::Captured:
		owner->piecesCaptured.insert(piece);
		break;
	case Piece::State_t::Held:
		owner->piecesHeld.insert(piece);
		break;
	default:
		ReportError("move error - piece has an invalid state: %i\n", state);
		return false;
	}

	piece->pieceState = state;
	piece->position = pos;
	piece->owner = owner;
	piece->pieceType = type;
	piece->customState = (piece->customState | addState) & ~removeState;

	if (!updateDisplay)
		return true;

	// move the svg element for this piece
#ifdef EMSCRIPTEN
	EM_ASM_ARGS(
		{ movePiece($0, $1, $2, $3, $4, $5) },
		piece->GetID(), (int)state, pos == 0 ? 0 : pos->GetCoordX(), pos == 0 ? 0 : pos->GetCoordY(), owner->GetID(), type->GetAppearance(owner)
	);
#endif

	return true;
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


MoveStep* MoveStep::CreateCapture(Piece *piece, Cell *from)
{
	MoveStep *step = new MoveStep(piece);
	step->fromState = Piece::State_t::OnBoard;
	step->toState = Piece::State_t::Captured;
	step->fromPos = from;
	step->distance = 0;
	return step;
};


MoveStep* MoveStep::CreatePickup(Piece *piece, Cell *from, Player *heldBy)
{
	MoveStep *step = new MoveStep(piece);
	step->fromState = Piece::State_t::OnBoard;
	step->toState = Piece::State_t::Held;
	step->fromPos = from;
	step->fromOwner = piece->GetOwner();
	step->toOwner = heldBy;
	step->distance = 0;
	return step;
};


MoveStep* MoveStep::CreateDrop(Piece *piece, Cell *to, Player *droppedBy)
{
	MoveStep *step = new MoveStep(piece);
	step->fromState = Piece::State_t::Held;
	step->toState = Piece::State_t::OnBoard;
	step->toPos = to;
	step->distance = 0;
	return step;
};


MoveStep* MoveStep::CreatePromotion(Piece *piece, PieceType *fromType, PieceType *toType)
{
	MoveStep *step = new MoveStep(piece);
	step->fromState = step->toState = piece->GetState();
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
	step->fromPos = step->toPos = piece->GetPosition();
	step->fromOwner = fromOwner;
	step->toOwner = toOwner;
	step->distance = 0;
	return step;
};


MoveStep* MoveStep::CreateAddState(Piece *piece, customstate_t state)
{
	MoveStep *step = new MoveStep(piece);
	step->fromState = step->toState = piece->GetState();
	step->fromPos = step->toPos = piece->GetPosition();
	step->distance = 0;
	step->addState = state;
	return step;
};


MoveStep* MoveStep::CreateRemoveState(Piece *piece, customstate_t state)
{
	MoveStep *step = new MoveStep(piece);
	step->fromState = step->toState = piece->GetState();
	step->fromPos = step->toPos = piece->GetPosition();
	step->distance = 0;
	step->removeState = state;
	return step;
};