#pragma once

#if _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#define new DEBUG_NEW
#else
	#include <stdlib.h>
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

void ReportError(const char *msg, ...);

#define NO_SVG

// if some new implementation wishes to use SVG output, then simply duplicate this logic
#ifdef EMSCRIPTEN
#undef NO_SVG
#endif

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

#define FOR_EACH_DIR_IN_SET(dirs, i) for (direction_t i = FIRST_ABSOLUTE_DIRECTION; i <= dirs; i*=2) if ((dirs & i) != 0)