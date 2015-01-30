#pragma once

class Distance
{
public:
	typedef enum { None, Max, Prev } RelativeTo_t;

	Distance(RelativeTo_t relativeTo, int number);
	~Distance();

	static Distance Any, Zero;
private:
	int number;
	RelativeTo_t relativeTo;
};