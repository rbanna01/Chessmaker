#include "PieceType.h"
#include "MoveDefinition.h"

PieceType::PieceType()
{
	capturedAs = this;
}


PieceType::~PieceType()
{
	while (!moves.empty())
		delete moves.front(), moves.pop_front();

#ifndef NO_SVG
	for (auto it = appearances.begin(); it != appearances.end(); it++)
		delete [] it->second;
#endif
}
