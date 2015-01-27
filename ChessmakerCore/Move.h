#pragma once

#include <list>

class Cell;
class GameState;
class MoveStep;
class Player;
class Piece;

class Move
{
public:
	Move(Player *player, GameState *prevState, Piece *piece, Cell *startPos);
	~Move();

	void AddStep(MoveStep *step) { steps.push_back(step); }

	Move *Clone();
	bool Perform(bool updateDisplay);
	bool Reverse(bool updateDisplay);
private:
	Player *player;
	GameState *prevState, *subsequentState;
	Piece *piece;
	Cell *startPos;
	char notation[10];

	std::list<MoveStep*> steps;

	int prevPieceMoveTurn;
};

