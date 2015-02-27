#include "MoveConditions.h"

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
	return false; // todo: implement this
}


bool MoveCondition_Type::IsSatisfied(Move *move)
{
	return false; // todo: implement this
}


bool MoveCondition_Owner::IsSatisfied(Move *move)
{
	return false; // todo: implement this
}


bool MoveCondition_MoveNumber::IsSatisfied(Move *move)
{
	return false; // todo: implement this
}


bool MoveCondition_MaxDist::IsSatisfied(Move *move)
{
	return false; // todo: implement this
}


bool MoveCondition_TurnsSinceLastMove::IsSatisfied(Move *move)
{
	return false; // todo: implement this
}


bool MoveCondition_Threatened::IsSatisfied(Move *move)
{
	return false; // todo: implement this
}