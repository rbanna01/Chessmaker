#include "StateConditions.h"

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
				if (any)
					return false;
				else
					any = true;
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
	if (MoveCondition_Threatened::alreadyChecking)
        return false; // assume NOT threatened

	/*var pieces = state.currentPlayer.piecesOnBoard;
    for (var i = 0; i < pieces.length; i++) {
        var piece = pieces[i];
        if (!piece.typeMatches(this.pieceType))
            continue;

        MoveCondition_Threatened::alreadyChecking = true;
        var threatened = MoveCondition_Threatened.IsThreatened(state, piece.position);
        MoveCondition_Threatened::alreadyChecking = false;

        if (threatened)
            return true;
    }

    return false;*/
	return false; // todo: implement this
}