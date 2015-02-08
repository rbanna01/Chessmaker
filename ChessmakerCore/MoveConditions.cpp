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