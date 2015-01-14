#pragma once

class Game;
class Player;

class GameState
{
public:
	GameState(Game *game, int turnNumber);
	~GameState();

private:
	Game *game;
	Player *currentPlayer;
	int turnNumber;

	friend class Move;
};

