#include "GameState.h"
#include "Game.h"

GameState::GameState(Game *game, int turnNumber)
{
	this->game = game;
	this->turnNumber = turnNumber;
}


GameState::~GameState()
{
	
}