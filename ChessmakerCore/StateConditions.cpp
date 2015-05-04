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


bool StateCondition_RepetitionOfPosition::IsSatisfied(GameState *state, bool canMove)
{
	state->DetermineHash(); // if we are to re-use states, we should only call this if the state doesn't already have a hash

	// use the hash stored on game states to count how many times states have occurred with the same hash
	int repetitionCount = IncrementCount(state);
	state->MarkForPositionRepetition();
	return ResolveComparison(comparison, repetitionCount, number);
}


int StateCondition_RepetitionOfPosition::IncrementCount(GameState *state)
{
	const char *stateKey = state->GetHash();
	auto it = positionRepetitions.find(stateKey);

	if (it == positionRepetitions.end())
	{
		positionRepetitions.insert(std::pair<const char*, int>(stateKey, 1));
		return 1;
	}
	else
		return ++it->second;
}


void StateCondition_RepetitionOfPosition::DecrementCount(GameState *state)
{
	const char *stateKey = state->GetHash();
	auto it = positionRepetitions.find(stateKey);

	if (it == positionRepetitions.end())
	{
		ReportError("Attempting to reduce the occurance of an unknown position's repetition");
		return;
	}

	if (--it->second <= 0) // erasing here ensures that we don't hold onto the key of a deleted state
		positionRepetitions.erase(it);
}


std::map<const char*, int, char_cmp> StateCondition_RepetitionOfPosition::positionRepetitions;