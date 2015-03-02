#pragma once

class Game;
class Player;

class GameState
{
public:
	GameState(Game *game, int turnNumber);
	~GameState();

	bool PrepareMovesForTurn();

private:
	Game *game;
	Player *currentPlayer;
	int turnNumber;

	friend class EndOfGame;
	friend class Game;
	friend class Move;
};

