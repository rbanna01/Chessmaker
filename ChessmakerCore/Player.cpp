#include "ai.h"
#include "Piece.h"
#include "Player.h"
#include "Game.h"
#include "Board.h"

int Player::nextID = 1;

Player::Player(Game *game, char *name, direction_t forwardDir)
{
	this->game = game;
	this->type = Local;
	this->forwardDir = forwardDir;
	strcpy(this->name, name);
	id = Player::nextID++;
	ai = 0;
}


Player::~Player()
{
	for (auto it = piecesOnBoard.begin(); it != piecesOnBoard.end(); it++)
		delete *it;

	for (auto it = piecesHeld.begin(); it != piecesHeld.end(); it++)
		delete *it;

	for (auto it = piecesCaptured.begin(); it != piecesCaptured.end(); it++)
		delete *it;

	if (ai != 0)
		delete ai;
}


void Player::SetAI(PlayerAI* ai)
{
	if (ai != this->ai && this->ai != 0)
		delete this->ai;
	this->ai = ai;

	if (ai != 0)
		SetType(AI);
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

