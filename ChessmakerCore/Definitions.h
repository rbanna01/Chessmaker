#pragma once

#include <list>
#include <map>
#include <set>
#include <string>

struct char_cmp {
	bool operator () (const char *a, const char *b) const
	{
		return strcmp(a, b) < 0;
	}
};

#define DIRECTION_FORWARD 1
#define DIRECTION_SAME 2
#define FIRST_ABSOLUTE_DIRECTION 4

typedef unsigned int direction_t;
