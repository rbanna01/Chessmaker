#pragma once

#include "Definitions.h"

class Game;
class Move;
class Piece;
class Player;

class GameState
{
public:
	GameState(Game *game, Player *currentPlayer, GameState *previousState);
	~GameState();

	std::list<Move*> *PrepareMovesForTurn();
	std::list<Move*> *DeterminePossibleMoves();
	std::list<Move*> *DetermineThreatMoves();
	Game *GetGame() { return game; }
	Player *GetCurrentPlayer() { return currentPlayer; }
	int GetTurnNumber() { return turnNumber; }
	GameState *GetPreviousState() { return previousState; }
	void ClearPreviousState() { previousState = 0; }
	Move *GetSubsequentMove() { return subsequentMove; }

private:
	Game *game;
	Player *currentPlayer;
	int turnNumber;
	GameState *previousState;
	Move *subsequentMove;

	void CalculateMovesForPlayer(Player *player, std::list<Move*> *output);

	friend class StateLogic;
	friend class Game;
	friend class Move;
};

