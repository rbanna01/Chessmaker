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
	hasCustomNotation = false;
	appendNotation[0] = 0;
}


Move::~Move()
{
	while (!steps.empty())
		delete steps.front(), steps.pop_front();
}


Move *Move::Clone()
{
	Move *move = new Move(player, prevState, piece, startPos);
	for (auto it = steps.begin(); it != steps.end(); it++)
	{
		MoveStep *step = *it;
		move->AddStep(new MoveStep(*step), false);
	}

	move->references = references;
	move->description = description;
	if (hasCustomNotation)
		move->notation = notation;
	move->hasCustomNotation = hasCustomNotation;
	strcpy(move->appendNotation, appendNotation);

	return move;
}


GameState *Move::Perform(bool updateDisplay)
{
	for (auto it = steps.begin(); it != steps.end(); it++)
	{
		MoveStep *step = *it;

		if (!step->Perform(updateDisplay)) // move failed, roll back
		{
			if (it == steps.begin())
				return 0;

			bool reversing = true;
			do
			{
				it--;
				reversing = it != steps.begin();

				step = *it;
				if (!step->Reverse(updateDisplay))
				{
					ReportError("Encountered an error rolling back a failed move\n");
					return 0;
				}
			} while (reversing);
			
			return 0;
		}
	}

	prevPieceMoveTurn = piece->lastMoveTurn;
	piece->lastMoveTurn = prevState->turnNumber;
	piece->moveNumber++;
	prevState->subsequentMove = this;

	return new GameState(prevState->game, prevState->game->GetTurnOrder()->GetNextPlayer(), prevState);
}


bool Move::Reverse(bool updateDisplay)
{
	piece->lastMoveTurn = prevPieceMoveTurn;
	piece->moveNumber--;
	prevState->game->GetTurnOrder()->StepBackward();
	if (prevState->subsequentMove == this)
		prevState->subsequentMove = 0;

	for (auto it = steps.rbegin(); it != steps.rend(); it++)
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
					ReportError("Encountered an error rolling back a failed move reversal\n");
					return false;
				}
			} while (reversing);

			return false;
		}
	}

	return true;
}


const char *Move::DetermineNotation(int detailLevel)
{
	// if I already have a notation set, just return that?
	if (hasCustomNotation)
		return notation.c_str();

    switch (detailLevel)
	{
        case 1:
			notation = piece->pieceType->GetNotation();
			
			if (IsDrop())
				notation += "*";
			
			if (IsCapture())
				notation += "x";
			
			notation += GetEndPos()->GetName();
            break;
        case 2:
			notation = piece->pieceType->notation;
			
			if (IsDrop())
				notation += "*";
			else
				notation += startPos->GetName();
			
			if (IsCapture())
				notation += "x";
			
			notation += GetEndPos()->GetName();
            break;
        default:
			notation = piece->pieceType->GetNotation();
			notation += "(";
			notation += std::to_string(piece->GetID());
			notation += ")";
			
			if (IsDrop())
				notation += "*";
			else
				notation += startPos->GetName(); 
			
			if (IsCapture())
				notation += "x";
			
			notation += GetEndPos()->GetName();
            break;
    }
	
	notation += appendNotation;
	return notation.c_str();
}


void Move::SetAppendNotation(const char *append)
{
	strcpy(appendNotation, append);
}


void Move::AddPieceReference(Piece *piece, const char *ref)
{
	auto it = references.find(ref);
	if (it != references.end())
		it->second = piece;
	else
		references.insert(std::pair<const char*, Piece*>(ref, piece));
}


Piece *Move::GetPieceByReference(const char *ref, Piece *self)
{
	if (strcmp(ref, "self") == 0)
		return self; // this is needed only by conditions code, it looks like. If they could be made to handle "self" separately, this wouldn't be needed.

	auto it = references.find(ref);
	if (it == references.end())
		return 0;
	return it->second;
}


Cell *Move::GetEndPos()
{
	for (auto it = steps.rbegin(); it != steps.rend(); it++)
	{
		MoveStep *step = *it;
		if (step->GetPiece() == piece && step->toState == Piece::OnBoard)
			return step->toPos;
	}

	// if this piece doesn't move, consider other pieces that it might move or capture
	for (auto it = steps.rbegin(); it != steps.rend(); it++)
	{
		MoveStep *step = *it;
		if (step->toState == Piece::OnBoard)
			return step->toPos;
		else if (step->fromState == Piece::OnBoard)
			return step->fromPos;
	}

    return startPos;
}


std::list<Piece*> *Move::GetAllCaptures()
{
	std::list<Piece*> *allCaptures = new std::list<Piece*>();

	for (auto it = steps.begin(); it != steps.end(); it++)
	{
		MoveStep *step = *it;
		if (step->toState != Piece::OnBoard)
			allCaptures->push_back(piece);
	}
	return allCaptures;
}


bool Move::IsCapture()
{
	for (auto it = steps.begin(); it != steps.end(); it++)
	{
		MoveStep *step = *it;
		if (step->GetPiece() != piece && step->toState != Piece::OnBoard)
			return true;
	}
    return false;
}


bool Move::IsDrop()
{
	MoveStep *step = *steps.begin();
	return step->GetFromState() == Piece::Held;
}


bool Move::WouldCapture(Cell* target)
{
	for (auto it = steps.begin(); it != steps.end(); it++)
	{
		MoveStep *step = *it;
		if (step->GetPiece() != piece && step->toState != Piece::OnBoard && step->fromState == Piece::OnBoard && step->fromPos == target)
			return true;
	}
	return false;
}


PieceType *Move::GetPromotionType()
{
	for (auto it = steps.rbegin(); it != steps.rend(); it++)
	{
		MoveStep *step = *it;
		if (step->GetPiece() == piece && step->toType != step->fromType)
			return step->toType;
	}
	return 0;
}