#include "StateConditions.h"
#include "Game.h"
#include "GameState.h"
#include "MoveConditions.h"
#include "Piece.h"
#include "Player.h"

StateConditionGroup::StateConditionGroup(GroupType_t type)
{
	this->type = type;
}


StateConditionGroup::~StateConditionGroup()
{
	std::list<StateCondition*>::iterator it = elements.begin();
	while (it != elements.end())
	{
		delete (*it);
		it++;
	}
}


bool StateConditionGroup::IsSatisfied(GameState *state, bool canMove)
{
	std::list<StateCondition*>::iterator it = elements.begin();
	bool any = false;

	switch (type) {
	case GroupType_t::And:
		while (it != elements.end())
		{
			if (!(*it)->IsSatisfied(state, canMove))
				return false;
			it++;
		}
		return true;
	case GroupType_t::Or:
		while (it != elements.end())
		{
			if ((*it)->IsSatisfied(state, canMove))
				return true;
			it++;
		}
		return false;
	case GroupType_t::Nand:
		while (it != elements.end())
		{
			if (!(*it)->IsSatisfied(state, canMove))
				return true;
			it++;
		}
		return false;
	case GroupType_t::Nor:
		while (it != elements.end())
		{
			if ((*it)->IsSatisfied(state, canMove))
				return false;
			it++;
		}
		return true;
	case GroupType_t::Xor:
		while (it != elements.end())
		{
			if ((*it)->IsSatisfied(state, canMove))
			{
				if (any)
					return false;
				else
					any = true;
			}
			it++;
		}
		return any;
	default: // there are no other types!
		return false;
	}
}


bool StateCondition_CannotMove::IsSatisfied(GameState *state, bool canMove)
{
	return !canMove;
}


bool StateCondition_Threatened::IsSatisfied(GameState *state, bool canMove)
{
	if (MoveCondition_Threatened::checkingThreat)
        return false; // assume NOT threatened

	auto pieces = state->GetCurrentPlayer()->GetPiecesOnBoard();
	for (auto it = pieces.begin(); it != pieces.end(); it++)
	{
		Piece *piece = *it;
		if (!piece->TypeMatches(type))
            continue;
		
        MoveCondition_Threatened::checkingThreat = true;
        bool threatened = MoveCondition_Threatened::IsThreatened(state, piece->GetPosition());
        MoveCondition_Threatened::checkingThreat = false;
		
        if (threatened)
            return true;
    }

	return false;
}


bool StateCondition_TurnsSinceLastMove::IsSatisfied(GameState *state, bool canMove)
{
	if (state->GetTurnNumber() <= number)
		return false;

	GameState *checkState = state->GetPreviousState();

	int stepsBack = -1;
	while (checkState != 0 && stepsBack <= number)
	{
		stepsBack++;
		Move *move = checkState->GetSubsequentMove();
		checkState = checkState->GetPreviousState();

		// move == 0, always. And if I stop the clearing of subsequentMove in Move::Reverse, move is still A DELETED OBJECT... argh.

		if (owner != Player::Any && state->GetCurrentPlayer()->GetRelationship(move->GetPlayer()) != owner)
			continue;

		if (!move->GetPiece()->TypeMatches(type))
			continue;

		if (ResolveComparison(comparison, stepsBack, number))
			return true;
	}

	return false;
}


bool StateCondition_TurnsSinceLastCapture::IsSatisfied(GameState *state, bool canMove)
{
	if (state->GetTurnNumber() <= number)
		return false;

	GameState *checkState = state->GetPreviousState();

	int stepsBack = -1;
	while (checkState != 0 && stepsBack <= number)
	{
		stepsBack++;
		Move *move = checkState->GetSubsequentMove();
		checkState = checkState->GetPreviousState();

		// check the actual captured piece(s), instead of the moved piece
		auto capturedPieces = move->GetAllCaptures();
		for (auto it = capturedPieces->begin(); it != capturedPieces->end(); it++)
		{
			Piece *piece = *it;
			if (owner != Player::Any && state->GetCurrentPlayer()->GetRelationship(piece->GetOwner()) != owner)
				continue;

			if (!piece->TypeMatches(type))
				continue;

			if (ResolveComparison(comparison, stepsBack, number))
				return true;
		}
		delete capturedPieces;
	}

	return false;
}


bool StateCondition_LastMoveRepetition::IsSatisfied(GameState *state, bool canMove)
{
	int steps = number * period;

	if (state->GetTurnNumber() <= steps)
		return false;

	// todo: game state should store move repetition count ... ? But how does it know about turn order etc? How do we handle complicated turn order?
	// do we just say Last N moves for each player should match the ones for that player only? Is that sufficient?
	// In a "two moves about" game, I'd treat both moves of a player as one.
	// In a variable turns game ... there's no well-defined meaning for this. Hmm.
	return false;
}