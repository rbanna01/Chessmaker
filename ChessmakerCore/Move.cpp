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

	subsequentState = new GameState(prevState->game, 0, prevState->turnNumber + 1);
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

	move->references = references;

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


char *Move::DetermineNotation(int detailLevel)
{
	int numPlayers = prevState->game->players.size();
	int displayNumber = (prevState->turnNumber + numPlayers + 1) / numPlayers;
	sprintf(notation, "%i", displayNumber);

	if (prevState->turnNumber % numPlayers == 1) // first player's move
		strcat(notation, ".  ");
	else
		strcat(notation, "...");

    switch (detailLevel)
	{
        case 1:
			strcat(notation, piece->pieceType->notation);
			if (IsCapture())
				strcat(notation, "x");
			strcat(notation, GetEndPos()->GetName());
            break;
        case 2:
			strcat(notation, piece->pieceType->notation);
			strcat(notation, startPos->GetName());
			if (IsCapture())
				strcat(notation, "x");
			strcat(notation, GetEndPos()->GetName());
            break;
        default:
			strcat(notation, piece->pieceType->notation);

			char strID[8];
			sprintf(strID, "(%i)", piece->GetID());
			strcat(notation, strID);

			strcat(notation, startPos->GetName());
			if (IsCapture())
				strcat(notation, "x");
			strcat(notation, GetEndPos()->GetName());
            break;
    }
	
    // add promotion letter onto the end
    /*var promotion = GetPromotionType();
    if (promotion != 0)
        strcat(notation, promotion->notation;*/

    // add a plus to the end if its check. add a hash if its checkmate.

    // how to account for castling? some special moves simply need to specify their own special notation, i guess...

	return notation;
}


void Move::AddPieceReference(Piece *piece, char *ref)
{
	auto it = references.find(ref);
	if (it != references.end())
		it->second = piece;
	else
		references.insert(std::pair<char*, Piece*>(ref, piece));
}


Piece *Move::GetPieceByReference(char *ref)
{
	if (strcmp(ref, "self") == 0)
		return piece;

	auto it = references.find(ref);
	if (it == references.end())
		return 0;
	return it->second;
}


Cell *Move::GetEndPos()
{
	auto it = steps.rbegin();
	while (it != steps.rend())
	{
		MoveStep *step = *it;
		if (step->GetPiece() == piece && step->toState == Piece::OnBoard)
			return step->toPos;
	}

	// if this piece doesn't move, consider other pieces that it might move or capture
	it = steps.rbegin();
	while (it != steps.rend())
	{
		MoveStep *step = *it;
		if (step->toState == Piece::OnBoard)
			return step->toPos;
		else if (step->fromState == Piece::OnBoard)
			return step->fromPos;
	}

    return startPos;
}


std::list<Cell*> Move::GetAllPositions()
{
	std::list<Cell*> allPositions;

	auto it = steps.begin();
	while (it != steps.end())
	{
		MoveStep *step = *it;
		if (step->GetPiece() == piece && step->toState == Piece::OnBoard)
			allPositions.push_back(step->toPos);
	}

	return allPositions;
}


bool Move::IsCapture()
{
	auto it = steps.begin();
	while (it != steps.end())
	{
		MoveStep *step = *it;
		if (step->GetPiece() != piece && step->toState != Piece::OnBoard)
			return true;
	}
    return false;
}


bool Move::WouldCapture(Cell* target)
{
	auto it = steps.begin();
	while (it != steps.end())
	{
		MoveStep *step = *it;
		if (step->GetPiece() != piece && step->toState != Piece::OnBoard && step->fromState == Piece::OnBoard && step->fromPos == target)
			return true;
	}
	return false;
}


PieceType *Move::GetPromotionType()
{
	auto it = steps.rbegin();
	while (it != steps.rend())
	{
		MoveStep *step = *it;
		if (step->GetPiece() == piece && step->toType != step->fromType)
			return step->toType;
	}
	return 0;
}