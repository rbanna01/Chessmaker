#include "PieceType.h"
#include "Player.h"
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


#ifndef NO_SVG
const char *PieceType::GetAppearance(Player *player)
{
	auto it = appearances.find(player->GetID());

	if (it == appearances.end())
		return "";
	
	return it->second;
}
#endif