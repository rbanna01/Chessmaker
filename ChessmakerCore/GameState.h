#pragma once

#include "Definitions.h"

class Game;
class Move;
class Piece;
class Player;

class GameState
{
public:
	GameState(Game *game, Player *currentPlayer, int turnNumber);
	~GameState();

	std::list<Move*> *PrepareMovesForTurn();
	std::list<Move*> *DeterminePossibleMoves();
	std::list<Move*> *DetermineThreatMoves();
	Game *GetGame() { return game; }
	Player *GetCurrentPlayer() { return currentPlayer; }

private:
	Game *game;
	Player *currentPlayer;
	int turnNumber;

	void CalculateMovesForPlayer(Player *player, std::list<Move*> *output);

	friend class EndOfGame;
	friend class Game;
	friend class Move;
};

