#include "GameState.h"
#include "Board.h"
#include "Cell.h"
#include "Game.h"
#include "Move.h"
#include "MoveConditions.h"
#include "MoveDefinition.h"
#include "Piece.h"
#include "StateConditions.h"

GameState::GameState(Game *game, Player *currentPlayer, GameState *previousState)
{
	this->game = game;
	this->currentPlayer = currentPlayer;
	this->turnNumber = previousState == 0 ? 1 : previousState->turnNumber + 1;
	this->previousState = previousState;
	subsequentMove = 0;
	possibleMoves = 0;
	countedForPositionRepetition = false;
}


GameState::~GameState()
{
	if (previousState != 0)
		delete previousState;

	if (subsequentMove != 0)
		delete subsequentMove;

	ClearPossibleMoves();
}


std::list<Move*> *GameState::GetPossibleMoves()
{
	if (possibleMoves == 0)
	{
		possibleMoves = new std::list<Move*>();
		CalculateMovesForPlayer(currentPlayer, possibleMoves);
	}

	return possibleMoves;
}


void GameState::ClearPossibleMoves(Move *dontDelete)
{
	if (possibleMoves == 0)
		return;

	for (auto it = possibleMoves->begin(); it != possibleMoves->end(); it++)
		if (*it != dontDelete)
			delete *it;

	delete possibleMoves;
	possibleMoves = 0;
}


std::list<Move*> *GameState::DetermineThreatMoves()
{
	std::list<Move*> *moves = new std::list<Move*>();

	for (auto it = game->players.begin(); it != game->players.end(); it++)
	{
		Player *opponent = *it;
		if (currentPlayer->GetRelationship(opponent) != Player::Enemy)
			continue;

		CalculateMovesForPlayer(opponent, moves);
	}

	return moves;
}


void GameState::CalculateMovesForPlayer(Player *player, std::list<Move*> *output)
{
	// get all valid move possibilities for each piece on the board
	for (auto itPiece = player->piecesOnBoard.begin(); itPiece != player->piecesOnBoard.end(); itPiece++)
	{
		Piece *piece = *itPiece;
		Move *moveTemplate = new Move(piece->GetOwner(), this, piece, piece->GetPosition());

		auto moves = piece->GetType()->GetMoves();
		for (auto it = moves.begin(); it != moves.end(); it++)
		{
			MoveDefinition *moveDef = *it;
			std::list<Move*> *possibilities = moveDef->DetermineNextSteps(moveTemplate, piece, 0);
			for (auto itMove = possibilities->begin(); itMove != possibilities->end(); itMove++)
			{
				Move *move = *itMove;
				
				if (!MoveCondition_Threatened::checkingThreat && game->AnyIllegalMovesSpecified())
				{
					GameState *subsequentState = move->Perform(false);
					GameEnd *result = game->CheckEndOfTurn(this);
					move->Reverse(false);
					subsequentState->DiscardState();
					delete subsequentState;

					if (result->GetType() == StateLogic::IllegalMove)
					{
						delete move;
						continue;
					}
                }

				std::string customNotation = moveDef->GetCustomNotation();
				if (!customNotation.empty())
					move->SetNotation(customNotation, true);

                output->push_back(move);
            }
			delete possibilities;
		}

		delete moveTemplate;
	}
	
    // now look at each held piece
	if (player->piecesHeld.empty())
      return;

	std::set<PieceType*> checkedTypes;
	auto cells = game->board->GetCells();
	for (auto itPiece = player->piecesHeld.begin(); itPiece != player->piecesHeld.end(); itPiece++)
	{
		Piece *piece = *itPiece;
		
		// we actually only need to look at one of each type. Because there's nothing to differentiate two held pieces of the same type. (custom states are cleared during pickup.)
		if (checkedTypes.find(piece->GetType()) != checkedTypes.end())
			continue;

		checkedTypes.insert(piece->GetType());

		for (auto itCell = cells.begin(); itCell != cells.end(); itCell++)
		{
			Cell *cell = *itCell;
			if (cell->GetPiece() != 0)
				continue;

			Move *move = new Move(piece->GetOwner(), this, piece, cell);
			move->AddStep(MoveStep::CreateDrop(piece, cell, player));

			if (game->dropConditions != 0 && !game->dropConditions->IsSatisfied(piece, move, 0))
			{
				delete move;
				continue;
			}

			if (!MoveCondition_Threatened::checkingThreat && game->AnyIllegalMovesSpecified())
			{
				GameState *subsequentState = move->Perform(false);
				GameEnd *result = game->CheckEndOfTurn(this);
				move->Reverse(false);
				subsequentState->DiscardState();
				delete subsequentState;

				if (result->GetType() == StateLogic::IllegalMove)
				{
					delete move;
					continue;
				}
			}

			output->push_back(move);
		}
	}
}


void GameState::DetermineHash()
{
	hash = std::to_string(currentPlayer->GetID());

	int numOnBoard = 0, numHeld = 0;
	auto players = game->GetPlayers();
	for (auto it = players.begin(); it != players.end(); it++)
	{
		Player *player = *it;
		numOnBoard += player->piecesOnBoard.size();
		numHeld += player->piecesHeld.size();
	}

	hash += '.';
	hash += std::to_string(numOnBoard);
	hash += '.';
	hash += std::to_string(numHeld);
	hash += '.';
	hash += std::to_string(GetPossibleMoves()->size());
	
	std::vector<Cell*> cells = game->GetBoard()->GetCells();
	for (auto it = cells.begin(); it != cells.end(); it++)
	{
		Cell *cell = *it;
		Piece *piece = cell->GetPiece();
		if (piece == 0)
			continue;

		hash += ".";
		hash += cell->GetName(); // unique ID?
		hash += ",";
		hash += piece->GetType()->GetName(); // unique ID?
		hash += "|";
		hash += std::to_string(piece->GetOwner()->GetID());
	}
	

	for (auto it = players.begin(); it != players.end(); it++)
	{
		Player *player = *it;
		for (auto it2 = player->piecesHeld.begin(); it2 != player->piecesHeld.end(); it2++)
		{
			Piece *piece = *it2;

			// these have to be in the same order. D'oh!
			hash += "|h";
			hash += piece->GetType()->GetName(); // unique ID?
			hash += ",";
			hash += std::to_string(piece->GetOwner()->GetID());
		}
	}
}


void GameState::DiscardState()
{
	// prevent deleting this state from deleting all the way through the state history
	previousState = 0;

	// remove this state's contribution to repetition of position
	if (countedForPositionRepetition)
		StateCondition_RepetitionOfPosition::DecrementCount(this);
}