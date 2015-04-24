#include "StateLogic.h"
#include "Game.h"
#include "GameState.h"
#include "StateConditions.h"
#include "TurnOrder.h"

StateLogic::StateLogic(bool startOfTurn)
{
	this->startOfTurn = startOfTurn;
}


StateLogic::~StateLogic()
{
	auto it = elements.begin();
	while (it != elements.end())
	{
		delete *it;
		it++;
	}
}


StateLogic *StateLogic::CreateDefault(bool startOfTurn)
{
	StateLogic *stateLogic = new StateLogic(startOfTurn);
	
	StateConditionGroup *conditions = new StateConditionGroup(Condition::And);
	GameEnd *outcome = new GameEnd();

	if (startOfTurn)
	{
		// conditions->elements.push_back(new StateCondition_PieceCount("self", 0, Condition::Equals)); todo: add this in, once it exists
		outcome->type = Lose;
		// message
		// appendNotation
	}
	else
	{
		conditions->elements.push_back(new StateCondition_CannotMove());
		outcome->type = Draw;
		// message
		// appendNotation
	}

	StateLogicBlock *block = new StateLogicBlock(conditions);

	StateLogic *ifTrue = new StateLogic(startOfTurn);
	ifTrue->elements.push_back(outcome);

    return stateLogic;
}


StateLogic::GameEnd_t StateLogic::Evaluate(GameState *state, bool canMove)
{
	// return Win/Lose/Draw/IllegalMove, or None if the game isn't over yet

	bool noNextPlayer = state->game->GetTurnOrder()->GetNextPlayer() == 0;
	state->game->GetTurnOrder()->StepBackward();

	auto it = elements.begin();
	while (it != elements.end())
	{
		StateLogicElement *element = *it;
		GameEnd_t result = element->Evaluate(state, canMove);
		if (result != None)
			return result;
		it++;
	}

	if (startOfTurn)
	{
		if (!canMove)
		{
			if (state->currentPlayer->piecesOnBoard.size() == 0)
				return Lose; // can't move and have no pieces. lose.
			else
				return Draw; // can't move, but have pieces. draw.
		}
	}
	else if (noNextPlayer) // if the next player is null, we've reached the end of the turn order
        return Draw;

    return None;
}


StateLogic::GameEnd_t GameEnd::Evaluate(GameState* state, bool canMove)
{
	// todo: handle message and appendNotation, somehow
	return type;
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


StateLogic::GameEnd_t StateLogicBlock::Evaluate(GameState *state, bool canMove)
{
	bool satisfied = this->conditions->IsSatisfied(state, canMove);
	StateLogic *resultantLogic = satisfied ? logicIfTrue : logicIfFalse;

	if (resultantLogic != 0)
		return resultantLogic->Evaluate(state, canMove);

	return StateLogic::None;
}