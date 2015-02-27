#include "MoveConditions.h"

bool MoveCondition::ResolveComparison(NumericComparison_t type, int val1, int val2)
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
	std::list<MoveCondition*>::iterator it = elements.begin();
	bool any = false;

	switch (type) {
	case GroupType_t::And:
		while (it != elements.end())
            if (!(*it)->IsSatisfied(move))
                return false;
        return true;
    case GroupType_t::Or:
		while (it != elements.end())
			if ((*it)->IsSatisfied(move))
                return true;
        return false;
    case GroupType_t::Nand:
		while (it != elements.end())
			if (!(*it)->IsSatisfied(move))
                return true;
        return false;
    case GroupType_t::Nor:
		while (it != elements.end())
			if ((*it)->IsSatisfied(move))
                return false;
        return true;
    case GroupType_t::Xor:
		while (it != elements.end())
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
	/*var other = move.getPieceByRef(this.of);
    if (other == null)
        throw "Piece reference not found: " + this.of;

    return other.pieceType.name == this.type;*/
	return false; // todo: implement this
}


bool MoveCondition_Owner::IsSatisfied(Move *move)
{
	/*var other = move.getPieceByRef(this.of);
    if (other == null)
        throw "Piece reference not found: " + this.of;

    if (this.relationship == Player.Relationship.Any)
        return true;

    return this.relationship == move.player.getRelationship(other.ownerPlayer);*/
	return false; // todo: implement this
}


bool MoveCondition_MoveNumber::IsSatisfied(Move *move)
{
	/*var other = move.getPieceByRef(this.of);
    if (other == null)
        throw "Piece reference not found: " + this.of;

    return Conditions.ResolveComparison(this.comparison, other.moveNumber, this.number);*/
	return false; // todo: implement this
}


bool MoveCondition_MaxDist::IsSatisfied(Move *move)
{
	/*var other = move.getPieceByRef(this.from);
    if (other == null)
        throw "Piece reference not found: " + this.from;

    var previousStep = move.steps.length > 0 ? move.steps[move.steps.length - 1] : null;
    var dirs = move.player.resolveDirection(this.dir, previousStep != null && previousStep.direction != null ? previousStep.direction : move.player.forwardDir);

    for (int i = 0; i < dirs.length; i++) {
        var maxDist = move.prevState.game.board.getMaxDistance(other.position, dirs[i]);
        if (Conditions.ResolveComparison(this.comparison, maxDist, this.number))
            return true;
    }
    return false;*/
	return false; // todo: implement this
}


bool MoveCondition_TurnsSinceLastMove::IsSatisfied(Move *move)
{
	/*var other = move.getPieceByRef(this.of);
    if (other == null)
        throw "Piece reference not found: " + this.of;

    return Conditions.ResolveComparison(this.comparison, move.prevState.moveNumber - other.lastMoveTurn, this.number);*/
	return false; // todo: implement this
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
	/*var step = move.steps[move.steps.length - 1]; // all steps except the current one will already have been performed

    if (this.start && step.fromState == Piece.State.OnBoard) {
        var threatened = Conditions_Threatened.isThreatened(move.prevState, step.fromPos);
        
        if (threatened != this.value)
            return false;
    }

    if (this.end && step.toState == Piece.State.OnBoard) {
        if (!step.perform(move.prevState.game, false))
            return false;

        var threatened = Conditions_Threatened.isThreatened(move.prevState, step.toPos);
        step.reverse(move.prevState.game, false);

        if (threatened != this.value)
            return false;
    }

    return true;*/
	return false; // todo: implement this
}

bool MoveCondition_Threatened::IsThreatened(GameState *state, Cell *position)
{
	/*var moves = state.determineThreatMoves();

    for (int i = 0; i < moves.length; i++)
        if (moves[i].wouldCapture(pos))
            return true;

    return false;*/
	return false; // todo: implement this
}