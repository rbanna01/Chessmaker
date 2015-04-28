#include "AI.h"
#include "Game.h"
#include "GameState.h"
#include "Move.h"
#include "Piece.h"
#include "PieceType.h"
#include "Player.h"
#include <limits.h>


Move *AI_Random::SelectMove()
{
	auto moves = game->GetPossibleMoves();
	int selectedIndex = rand() % moves->size(), index = 0;
	
	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		if (index == selectedIndex)
			return *it;

		index++;
	}

	return 0;
}


Move *AI_RandomCapture::SelectMove()
{
	auto moves = game->GetPossibleMoves();
	int numCaptures = 0;
	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		Move *move = *it;
		if (move->IsCapture())
			numCaptures++;
	}

	if (numCaptures == 0)
		return AI_Random::SelectMove();

	int selectedIndex = rand() % numCaptures, index = 0;

	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		Move *move = *it;
		if (!move->IsCapture())
			continue;

		if (index == selectedIndex)
			return *it;

		index++;
	}

	return 0;
}

#define MAX_VAL INT_MAX
#define MIN_VAL -MAX_VAL
// the order we look through moves is important. If we look at the best moves first, we can ignore most others! See https://chessprogramming.wikispaces.com/Move+Ordering (consider captures first?)
// rather than just evaluating, we probably want to do a quiescence search, see https://chessprogramming.wikispaces.com/Quiescence+Search
Move *AI_AlphaBeta::SelectMove()
{
	int alpha = MIN_VAL;
	int beta = MAX_VAL;

	int bestScore = MIN_VAL;
	std::list<Move*> bestMoves;
    
	auto moves = game->GetPossibleMoves();
	SortMoves(moves);

	GameState *currentState = game->GetCurrentState();
	Player *player = currentState->GetCurrentPlayer();

	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		Move *move = *it;
		GameState *subsequentState = move->Perform(false);

		int score;
		if (player->GetRelationship(subsequentState->GetCurrentPlayer()) == Player::Enemy)
			score = -FindBestScore(currentState, subsequentState, -beta, -alpha, ply - 1);
		else
			score = FindBestScore(currentState, subsequentState, alpha, beta, ply - 1);

		move->Reverse(false);
		delete subsequentState;

		if (score > bestScore)
		{
			bestScore = score;

			bestMoves.clear();
			bestMoves.push_back(move);
		}
		else if (score == bestScore)
			bestMoves.push_back(move);
    }
	
	//srand(time(NULL)); // need to seed the random selection ... across all AIs. somehow.
	int selectedIndex = rand() % bestMoves.size(), index = 0;
	for (auto it = bestMoves.begin(); it != bestMoves.end(); it++)
	{
		if (index == selectedIndex)
			return *it;

		index++;
	}
	
	return 0;
}


int AI_AlphaBeta::FindBestScore(GameState *prevState, GameState *currentState, int alpha, int beta, int depth)
{
	GameEnd *gameEnd = game->CheckEndOfTurn(prevState);
	if (gameEnd->GetType() != StateLogic::None)
		return GetScoreForStateLogic(gameEnd->GetType());

	if (depth == 0)
		return EvaluateBoard(currentState); // should be quiescing here

	auto moves = currentState->DeterminePossibleMoves();
	SortMoves(moves);

	gameEnd = game->CheckStartOfTurn(currentState, !moves->empty());
	if (gameEnd->GetType() != StateLogic::None)
	{
		for (auto it = moves->begin(); it != moves->end(); it++)
			delete *it;
		delete moves;
		return GetScoreForStateLogic(gameEnd->GetType());
	}
	
	int bestScore = MIN_VAL;
	Player *movePlayer = currentState->GetCurrentPlayer();

	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		Move *move = *it;
		GameState *subsequentState = move->Perform(false);
		int score;

		if (movePlayer->GetRelationship(subsequentState->GetCurrentPlayer()) == Player::Enemy)
			score = -FindBestScore(currentState, subsequentState, -beta, -alpha, depth - 1);
		else
			score = FindBestScore(currentState, subsequentState, alpha, beta, depth - 1);

		move->Reverse(false);
		if (score >= beta)
		{
			bestScore = score;

			for (; it != moves->end(); it++)
				delete *it;
			break;
		}
		else if (score > bestScore)
		{
			bestScore = score;
			if (score > alpha)
				alpha = score;
		}
		delete move;
	}

	delete moves;
	return bestScore;
}


void AI_AlphaBeta::SortMoves(std::list<Move*> *moves)
{
	// separate captures from non-capturing moves, and put them all at the front
	std::list<Move*> captures;
	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		Move *move = *it;
		if (move->IsCapture())
		{
			captures.push_front(move);
			it = moves->erase(it);
		}
	}

	for (auto it = captures.begin(); it != captures.end(); it++)
		moves->push_front(*it);
}


int AI_AlphaBeta::GetScoreForStateLogic(StateLogic::GameEnd_t result)
{
	switch (result)
	{
	case StateLogic::Win:
		return MAX_VAL;
	case StateLogic::Lose:
		return MIN_VAL;
	case StateLogic::Draw:
	default:
		return 0;
	}
}


int AI_AlphaBeta::EvaluateBoard(GameState *state)
{
	int score = 0;

	Player *current = state->GetCurrentPlayer();

	auto players = game->GetPlayers();
	for (auto itPlayers = players.begin(); itPlayers != players.end(); itPlayers++)
    {
		Player *other = *itPlayers;
		int playerScore = 0;

		// add piece value to score
		auto piecesOnBoard = other->GetPiecesOnBoard();
		for (auto itPieces = piecesOnBoard.begin(); itPieces != piecesOnBoard.end(); itPieces++)
		{
			Piece *piece = *itPieces;
			playerScore += piece->GetType()->GetValue();

            // calculating possible moves (again?) here is gonna slow things down more

            // NOTE! Neither of these would be cached! they ought to be!

            // one simple option
            //playerScore += 0.1 * move.subsequentState.determineMovesForPiece(piece).length;

            // more complicated
            //var moves = move.subsequentState.determineMovesForPiece(piece);
            //for (var k = 0; k < moves.length; k++)
            //    playerScore += moves[k].isCapture() ? 0.2 : 0.1;
		}

		if (current->GetRelationship(other) == Player::Enemy)
			score -= playerScore;
		else
			score += playerScore;
	}

	return score;
}