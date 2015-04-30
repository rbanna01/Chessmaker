#include "StateLogic.h"
#include "Game.h"
#include "GameState.h"
#include "StateConditions.h"
#include "TurnOrder.h"

GameEnd *noEnd, *outOfPieces, *outOfMoves, *endOfTurnOrder;

StateLogic::StateLogic(bool startOfTurn, bool root)
{
	this->startOfTurn = startOfTurn;
	this->root = root;

	if (root && startOfTurn)
	{
		noEnd = new GameEnd(None, "");
		outOfPieces = new GameEnd(Lose, "out of pieces");
		outOfMoves = new GameEnd(Draw, "no moves possible");
		endOfTurnOrder = new GameEnd(Draw, "end of game reached");
	}
}


StateLogic::~StateLogic()
{
	auto it = elements.begin();
	while (it != elements.end())
	{
		delete *it;
		it++;
	}

	if (root && startOfTurn)
	{
		delete noEnd;
		delete outOfPieces;
		delete outOfMoves;
		delete endOfTurnOrder;
	}
}


GameEnd* StateLogic::Evaluate(GameState *state, bool canMove)
{
	// return a GameEnd with a type of Win/Lose/Draw/IllegalMove, or None if the game isn't over yet. It may also have a message, and an operation to peform on the move's notation.

	auto it = elements.begin();
	while (it != elements.end())
	{
		StateLogicElement *element = *it;
		GameEnd *result = element->Evaluate(state, canMove);
		if (result != 0)
			return result;
		it++;
	}

	if (root)
		return noEnd;

	if (startOfTurn)
	{
		if (!canMove)
		{
			if (state->currentPlayer->piecesOnBoard.size() == 0)
				return outOfPieces; // can't move and have no pieces. lose.
			else
				return outOfMoves; // can't move, but have pieces. draw.
		}
	}
	else
	{
		bool noNextPlayer = state->game->GetTurnOrder()->GetNextPlayer() == 0;
		state->game->GetTurnOrder()->StepBackward();

		if (noNextPlayer)
			return endOfTurnOrder; // if the next player is null, we've reached the end of the turn order
	}

    return noEnd;
}


StateLogicBlock::StateLogicBlock(StateConditionGroup *conditions)
{
	this->conditions = conditions;
	logicIfTrue = logicIfFalse = 0;
}


StateLogicBlock::~StateLogicBlock()
{
	delete conditions;
	if (logicIfTrue != 0)
		delete logicIfTrue;
	if (logicIfFalse != 0)
		delete logicIfFalse;
}


GameEnd::GameEnd(StateLogic::GameEnd_t type, const char *message, const char *appendNotation)
{
	this->type = type;
	sprintf(this->message, message);
	sprintf(this->appendNotation, appendNotation);
}


GameEnd* StateLogicBlock::Evaluate(GameState *state, bool canMove)
{
	bool satisfied = this->conditions->IsSatisfied(state, canMove);
	StateLogic *resultantLogic = satisfied ? logicIfTrue : logicIfFalse;

	if (resultantLogic != 0)
		return resultantLogic->Evaluate(state, canMove);

	return 0;
}