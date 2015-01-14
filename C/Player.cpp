#include "Constants.h"
#include "Player.h"
#include "Game.h"
#include "Board.h"

Player::Player(Game *game, int id, PlayerType_t type, int forwardDir)
{
	this->game = game;
	this->id = id;
	this->type = type;
	this->forwardDir = forwardDir;
	
}


Player::~Player()
{
}


int Player::ResolveDirection(int dir, int prevDir)
{
	if (dir == DIRECTION_FORWARD)
		return /*[*/forwardDir/*]*/;

	return game->GetBoard()->ResolveDirection(dir, prevDir);
}


PlayerRelationship_t Player::GetRelationship(Player *other)
{
	if (this == other)
		return Self;
	else
		return Enemy;
}

