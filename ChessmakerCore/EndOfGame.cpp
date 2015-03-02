#include "EndOfGame.h"
#include "Game.h"
#include "GameState.h"
#include "StateConditions.h"
#include "TurnOrder.h"

EndOfGame::EndOfGame()
{
	illegalMovesSpecified = false;
}


EndOfGame::~EndOfGame()
{
	while (!startOfTurnChecks.empty())
		delete startOfTurnChecks.front(), startOfTurnChecks.pop_front();

	while (!endOfTurnChecks.empty())
		delete endOfTurnChecks.front(), endOfTurnChecks.pop_front();
}


EndOfGame *EndOfGame::CreateDefault()
{
	EndOfGame *endOfGame = new EndOfGame();

	StateConditionGroup *conditions = new StateConditionGroup(Condition::And);
	// conditions->elements.push_back(new StateCondition_PieceCount("self", 0, Condition::Equals)); todo: add this in, once it exists
	EndOfGameCheck *outOfPieces = new EndOfGameCheck(Lose, conditions);
    endOfGame->startOfTurnChecks.push_back(outOfPieces);

	conditions = new StateConditionGroup(Condition::And);
    conditions->elements.push_back(new StateCondition_CannotMove());
	EndOfGameCheck *outOfMoves = new EndOfGameCheck(Draw, conditions);
	endOfGame->startOfTurnChecks.push_back(outOfMoves);

    return endOfGame;
}


EndOfGame::CheckType_t EndOfGame::CheckStartOfTurn(GameState *state, bool canMove)
{
	// return Win/Lose/Draw, or None if the game isn't over yet
	auto it = startOfTurnChecks.begin();
	while (it != startOfTurnChecks.end())
	{
		EndOfGameCheck *check = *it;
		if (check->conditions->IsSatisfied(state, canMove))
			return check->type;
		it++;
	}

    if (canMove)
        return None;

    if (state->currentPlayer->piecesOnBoard.size() == 0)
        return EndOfGame::Lose; // can't move and have no pieces. lose.

	return EndOfGame::Draw; // can't move, but have pieces. draw.
}


EndOfGame::CheckType_t EndOfGame::CheckEndOfTurn(GameState *state, Move *move)
{
	// return Win/Lose/Draw/IllegalMove, or None if the game isn't over yet

	bool noNextPlayer = state->game->GetTurnOrder()->GetNextPlayer() == 0;
	state->game->GetTurnOrder()->StepBackward();

	auto it = endOfTurnChecks.begin();
	while (it != endOfTurnChecks.end())
	{
		EndOfGameCheck *check = *it;
		if (check->conditions->IsSatisfied(state, true))
			return check->type;
		it++;
	}

    // if the next player is null, we've reached the end of the turn order
    if (noNextPlayer)
        return Draw;

    return None;
}