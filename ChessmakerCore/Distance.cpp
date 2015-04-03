#include "Distance.h"
#include "MoveStep.h"

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


int Distance::GetValue(MoveStep *previousStep, int maxDist)
{
	switch (relativeTo)
	{
	case None:
		return this == &Any ? 1 : number; // the minimum for "any" is 1, and the max will be maxDist (from getRange)
	case Max:
		return maxDist + number;
	case Prev:
		if (previousStep != 0)
			return previousStep->distance + number;
		return number;
	default:
		ReportError("Distance has unexpected relative-to value: %i\n", (int)relativeTo);
		return 0;
	}
}


int Distance::GetMaxValue(Distance *dist2, MoveStep *previousStep, int maxDist)
{
	if (this == &Any)
		return maxDist;

	if (dist2 == 0)
		return GetValue(previousStep, maxDist);
	
	return dist2->GetValue(previousStep, maxDist);
}