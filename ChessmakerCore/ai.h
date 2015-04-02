#pragma once

#include "Definitions.h"
#include "EndOfGame.h"

class Game;
class GameState;
class Move;

class PlayerAI
{
public:
	PlayerAI(Game *game) { this->game = game; }
	virtual ~PlayerAI() {	}

	virtual Move *SelectMove() = 0;
protected:
	Game *game;
};


class AI_Random : public PlayerAI
{
public:
	AI_Random(Game *game) : PlayerAI(game) { }
	virtual Move *SelectMove();
};


class AI_RandomCapture : public AI_Random
{
public:
	AI_RandomCapture(Game *game) : AI_Random(game) { }
	virtual Move *SelectMove();
};


class AI_AlphaBeta : public PlayerAI
{
public:
	AI_AlphaBeta(Game *game, int ply) : PlayerAI(game) { this->ply = ply; }
	virtual Move *SelectMove();
private:
	double FindBestScore(GameState *state, double alpha, double beta, int depth);
	double GetMoveScore(Move *move, double alpha, double beta, int depth);
	double GetScoreForEndOfGame(EndOfGame::CheckType_t result);
	double EvaluateBoard(GameState *state);
	int ply;
};