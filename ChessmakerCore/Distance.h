#pragma once

class MoveStep;

class Distance
{
public:
	typedef enum { None, Max, Prev } RelativeTo_t;

	Distance(RelativeTo_t relativeTo, int number);
	~Distance();

	int GetValue(MoveStep *previousStep, int maxDist);
	int GetMaxValue(Distance *dist2, MoveStep *previousStep, int maxDist);

	static Distance Any, Zero;

	bool operator==(const Distance& other)
	{
		return number == other.number && relativeTo == other.relativeTo;
	}

	bool operator!=(const Distance& other)
	{
		return !(*this == other);
	}

private:
	int number;
	RelativeTo_t relativeTo;
};