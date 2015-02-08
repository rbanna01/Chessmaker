#include "Move.h"
#include "MoveStep.h"
#include "Game.h"
#include "GameState.h"
#include "TurnOrder.h"

Move::Move(Player *player, GameState *prevState, Piece *piece, Cell *startPos)
{
	this->player = player;
	this->prevState = prevState;
	this->piece = piece;
	this->startPos = startPos;
}


Move::~Move()
{
	std::list<MoveStep*>::iterator it = steps.begin();
	while (it != steps.end())
	{
		delete (*it);
		it++;
	}
}


Move *Move::Clone()
{
	Move *move = new Move(player, prevState, piece, startPos);
	std::list<MoveStep*>::iterator it = steps.begin();
	while (it != steps.end())
	{
		move->AddStep(*it);
		it++;
	}

	// todo: also copy references

	return move;
}


bool Move::Perform(bool updateDisplay)
{
	std::list<MoveStep*>::iterator it = steps.begin();
	while (it != steps.end())
	{
		MoveStep *step = *it;

		if (!step->Perform(updateDisplay)) // move failed, roll back
		{
			if (it == steps.begin())
				return false;

			bool reversing = true;
			do
			{
				it--;
				reversing = it != steps.begin();

				step = *it;
				if (!step->Reverse(updateDisplay))
				{
					// todo: report cockup somehow
					return false;
				}
			} while (reversing);
			
			return false;
		}

		it++;
	}

	prevPieceMoveTurn = piece->lastMoveTurn;
	piece->lastMoveTurn = prevState->turnNumber;
	piece->moveNumber++;
	subsequentState->currentPlayer = prevState->game->GetTurnOrder()->GetNextPlayer(); // while this function ought to be called here, it isn't a sensible place to set the state's player. That should surely be done on creation?
	return true;
}


bool Move::Reverse(bool updateDisplay)
{
	std::list<MoveStep*>::reverse_iterator it = steps.rbegin();
	while (it != steps.rend())
	{
		MoveStep *step = *it;

		if (!step->Reverse(updateDisplay)) // move failed, roll back
		{
			if (it == steps.rbegin())
				return false;

			bool reversing = true;
			do
			{
				it--;
				reversing = it != steps.rbegin();

				step = *it;
				if (!step->Perform(updateDisplay))
				{
					// todo: report cockup somehow
					return false;
				}
			} while (reversing);

			return false;
		}

		it++;
	}

	piece->lastMoveTurn = prevPieceMoveTurn;
    piece->moveNumber--;
	prevState->game->GetTurnOrder()->StepBackward();
	return true;
}

