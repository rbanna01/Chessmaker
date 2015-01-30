#include "Distance.h"

#define DISTANCE_ANY -1
Distance Distance::Any = Distance(None, DISTANCE_ANY);
Distance Distance::Zero = Distance(None, 0);


Distance::Distance(RelativeTo_t relativeTo, int number)
{
	this->relativeTo = relativeTo;
	this->number = number;
}


Distance::~Distance()
{
	
}