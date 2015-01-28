#include "Constants.h"
#include "Player.h"
#include "Game.h"
#include "Board.h"

Player::Player(Game *game, int id, Type_t type, int forwardDir)
{
	this->game = game;
	this->id = id;
	this->type = type;
	this->forwardDir = forwardDir;
	
}


Player::~Player()
{
}


unsigned int Player::ResolveDirections(unsigned int dir, unsigned int prevDir)
{
	if (dir == DIRECTION_FORWARD)
		return forwardDir;

	return game->GetBoard()->ResolveDirections(dir, prevDir);
}


Player::Relationship_t Player::GetRelationship(Player *other)
{
	if (this == other)
		return Self;
	else
		return Enemy;
}

