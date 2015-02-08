#pragma once

#include <list>

class MoveCondition
{
public:
	typedef enum { Equals, LessThan, GreaterThan, LessThanOrEquals, GreaterThanOrEquals } NumericComparison_t;
};

class MoveConditionGroup : public MoveCondition
{
public:
	typedef enum { And, Or, Nand, Nor, Xor } GroupType_t;

	MoveConditionGroup(GroupType_t type);
	~MoveConditionGroup();

private:
	GroupType_t type;
	std::list<MoveCondition*> elements;

	friend class GameParser;
};

