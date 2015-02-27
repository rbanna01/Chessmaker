#include "Constants.h"
#include "Player.h"
#include "Game.h"
#include "Board.h"

int Player::nextID = 1;

Player::Player(Game *game, char *name, direction_t forwardDir)
{
	this->game = game;
	this->type = Local;
	this->forwardDir = forwardDir;
	strncpy(this->name, name, PLAYER_NAME_LENGTH);
	this->id = Player::nextID++;
}


Player::~Player()
{
	for (auto it = piecesOnBoard.begin(); it != piecesOnBoard.end(); it++)
		delete it->second;

	for (auto it = piecesHeld.begin(); it != piecesHeld.end(); it++)
		delete it->second;

	for (auto it = piecesCaptured.begin(); it != piecesCaptured.end(); it++)
		delete it->second;
}


direction_t Player::ResolveDirections(direction_t dir, direction_t prevDir)
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

