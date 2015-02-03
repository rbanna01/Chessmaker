#include "Constants.h"
#include "Player.h"
#include "Game.h"
#include "Board.h"

int Player::nextID = 1;

Player::Player(Game *game, char *name, unsigned int forwardDir)
{
	this->game = game;
	this->type = Local;
	this->forwardDir = forwardDir;
	strncpy(this->name, name, PLAYER_NAME_LENGTH);
	this->id = Player::nextID++;
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

