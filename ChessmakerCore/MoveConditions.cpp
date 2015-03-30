#include "MoveConditions.h"
#include "Board.h"
#include "Game.h"
#include "GameState.h"
#include "Move.h"
#include "MoveStep.h"

bool Condition::ResolveComparison(NumericComparison_t type, int val1, int val2)
{
	switch (type) {
	case NumericComparison_t::Equals:
		return val1 == val2;
	case NumericComparison_t::LessThan:
		return val1 < val2;
	case NumericComparison_t::GreaterThan:
		return val1 > val2;
	case NumericComparison_t::LessThanOrEquals:
		return val1 <= val2;
	case NumericComparison_t::GreaterThanOrEquals:
		return val1 >= val2;
	default:
		return false;
	}
}


MoveConditionGroup::MoveConditionGroup(GroupType_t type)
{
	this->type = type;
}


MoveConditionGroup::~MoveConditionGroup()
{
	std::list<MoveCondition*>::iterator it = elements.begin();
	while (it != elements.end())
	{
		delete (*it);
		it++;
	}
}


bool MoveConditionGroup::IsSatisfied(Move *move)
{
	bool any = false;

	switch (type) {
	case GroupType_t::And:
		for (auto it = elements.begin(); it != elements.end(); it++)
            if (!(*it)->IsSatisfied(move))
                return false;
        return true;
    case GroupType_t::Or:
		for (auto it = elements.begin(); it != elements.end(); it++)
			if ((*it)->IsSatisfied(move))
                return true;
        return false;
    case GroupType_t::Nand:
		for (auto it = elements.begin(); it != elements.end(); it++)
			if (!(*it)->IsSatisfied(move))
                return true;
        return false;
    case GroupType_t::Nor:
		for (auto it = elements.begin(); it != elements.end(); it++)
			if ((*it)->IsSatisfied(move))
                return false;
        return true;
    case GroupType_t::Xor:
		for (auto it = elements.begin(); it != elements.end(); it++)
			if ((*it)->IsSatisfied(move))
                if (any)
                    return false;
                else
                    any = true;
        return any;
	default: // there are no other types!
		return false;
    }
}


bool MoveCondition_Type::IsSatisfied(Move *move)
{
	Piece *other = move->GetPieceByReference(pieceRef);
	if (other == 0)
	{
		// todo: report piece reference not found: pieceRef
		return false;
	}
	return other->TypeMatches(type);
}


bool MoveCondition_Owner::IsSatisfied(Move *move)
{
	Piece *other = move->GetPieceByReference(pieceRef);
	if (other == 0)
	{
		// todo: report piece reference not found: pieceRef
		return false;
	}

	if (relationship == Player::Any)
		return true;

	return relationship == move->GetPlayer()->GetRelationship(other->GetOwner());
}


bool MoveCondition_MoveNumber::IsSatisfied(Move *move)
{
	Piece *other = move->GetPieceByReference(pieceRef);
	if (other == 0)
	{
		// todo: report piece reference not found: pieceRef
		return false;
	}

    return ResolveComparison(comparison, other->GetMoveNumber(), number);
}


bool MoveCondition_MaxDist::IsSatisfied(Move *move)
{
	Piece *other = move->GetPieceByReference(pieceRef);
	if (other == 0)
	{
		// todo: report piece reference not found: pieceRef
		return false;
	}

	auto steps = move->GetSteps();
	MoveStep *previousStep = steps.empty() ? 0 : *steps.rbegin();
	direction_t dirs = move->GetPlayer()->ResolveDirections(dir, previousStep != 0 && previousStep->GetDirection() != 0 ? previousStep->GetDirection() : move->GetPlayer()->GetForwardDirection());
	Board *board = move->GetPrevState()->GetGame()->GetBoard();

	FOR_EACH_DIR_IN_SET(dirs, dir)
	{
        int maxDist = board->GetMaxDistance(other->GetPosition(), dir);
        if (ResolveComparison(comparison, maxDist, number))
            return true;
    }
    return false;
}


bool MoveCondition_TurnsSinceLastMove::IsSatisfied(Move *move)
{
	Piece *other = move->GetPieceByReference(pieceRef);
	if (other == 0)
	{
		// todo: report piece reference not found: pieceRef
		return false;
	}

    return ResolveComparison(comparison, move->GetPrevState()->GetTurnNumber() - other->GetLastMoveTurn(), number);
}


bool MoveCondition_Threatened::alreadyChecking = false; // when performing moves to check this, don't go performing other moves for other "threatened" checks, or things get messy
bool MoveCondition_Threatened::IsSatisfied(Move *move)
{
	if (alreadyChecking)
		return true;

	alreadyChecking = true;
	bool retVal = CheckSatisfied(move);
	alreadyChecking = false;

	return retVal;
}

bool MoveCondition_Threatened::CheckSatisfied(Move *move)
{
	MoveStep *step = *move->GetSteps().rbegin(); // all steps except the current one will already have been performed

	if (start && step->GetFromState() == Piece::OnBoard)
	{
		bool threatened = IsThreatened(move->GetPrevState(), step->GetFromPosition());
		if (threatened != value)
			return false;
	}

	if (end && step->GetToState() == Piece::OnBoard)
	{
		if (!step->Perform(false))
			return false;

		bool threatened = IsThreatened(move->GetPrevState(), step->GetToPosition());
		step->Reverse(false);

		if (threatened != value)
			return false;
	}

	return true;
}

bool MoveCondition_Threatened::IsThreatened(GameState *state, Cell *position)
{
	auto moves = state->DetermineThreatMoves();
	
	bool retVal = false;
	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		Move *move = *it;
		if (move->WouldCapture(position))
		{
			retVal = true;
			break;
		}
	}

	while (!moves->empty())
		delete moves->front(), moves->pop_front();
	delete moves;
	
	return retVal;
}