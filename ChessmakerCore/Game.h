#pragma once

class Board;
class EndOfGame;
class GameState;
class TurnOrder;

class Game
{
public:
	Game();
	~Game();

	Board *GetBoard() { return board; }
	GameState *GetCurrentState() { return currentState; }
	TurnOrder *GetTurnOrder() { return turnOrder; }
	EndOfGame *GetEndOfGame() { return endOfGame; }

private:
	Board *board;
	GameState *currentState;
	TurnOrder *turnOrder;
	EndOfGame *endOfGame;
	
	friend class GameParser;
};

