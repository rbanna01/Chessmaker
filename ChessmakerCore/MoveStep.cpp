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

	Place(toState, toStateOwner, toPos, toOwner, toType);

	if (updateDisplay)
		UpdateDisplay();

	return true;
}


bool MoveStep::Reverse(bool updateDisplay)
{
	if (!Pickup(toState, toStateOwner, toPos, toOwner, toType))
		return false;

	Place(fromState, fromStateOwner, fromPos, fromOwner, fromType);

	if (updateDisplay)
		UpdateDisplay();

	return true;
}


bool MoveStep::Pickup(Piece::State_t state, Player *stateOwner, Cell *pos, Player *owner, PieceType *type)
{
	if (piece->GetState() != state) // piece isn't in the expected state, quit
	{
		//console.log('state is wrong: got ' + this.piece.pieceState + ', expected ' + state + ' for ' + this.piece.ownerPlayer.name + ' ' + this.piece.pieceType.name);
		return false;
	}

	if (piece->GetOwner() != owner)
	{
		//console.log('owner is wrong: got ' + this.piece.ownerPlayer.name + ', expected ' + owner.name + ' for ' + this.piece.ownerPlayer.name + ' ' + this.piece.pieceType.name);
		return false;
	}

	if (piece->GetStateOwner() != stateOwner)
	{
		//console.log('state owner is wrong: got ' + (this.piece.stateOwner == null ? '[null]' : this.piece.stateOwner.name) + ', expected ' + (stateOwner == null ? '[null]' : stateOwner.name) + ' for ' + this.piece.ownerPlayer.name + ' ' + this.piece.pieceType.name);
		return false;
	}

	switch (state)
	{
	case Piece::OnBoard:
		if (piece->GetPosition() != pos)
		{
			//console.log('position is wrong: got ' + this.piece.position.name + ', expected ' + pos.name + ' for ' + this.piece.ownerPlayer.name + ' ' + this.piece.pieceType.name);
			return false;
		}
		if (toState != fromState || fromOwner != toOwner)
			owner->piecesOnBoard.erase(piece);
		pos->piece = 0;
		return true;

	case Piece::Captured:
		if (stateOwner == 0 || stateOwner->piecesCaptured.erase(piece) == 0)
			return false; // wasn't captured by that player after all ... can't perform this action
		return true;

	case Piece::Held:
		if (stateOwner == 0 || stateOwner->piecesHeld.erase(piece) == 0)
			return false; // wasn't held by that player after all ... can't perform this action
		return true;
	default:
		return false;// throw 'Unexpected piece state in MoveStep.pickup: ' + state;
	}
}


void MoveStep::Place(Piece::State_t state, Player *stateOwner, Cell *pos, Player *owner, PieceType *type)
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
		return; //throw 'Unexpected piece state in MoveStep.place: ' + state;
	}
}


void MoveStep::UpdateDisplay()
{
	// todo ... implement this somehow
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