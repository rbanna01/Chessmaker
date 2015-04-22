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


// the order we look through moves is important. If we look at the best moves first, we can ignore most others! See https://chessprogramming.wikispaces.com/Move+Ordering (consider captures first?)
// rather than just evaluating, we probably want to do a quiescence search, see https://chessprogramming.wikispaces.com/Quiescence+Search
Move *AI_AlphaBeta::SelectMove()
{
	int alpha = INT_MIN;
	int beta = INT_MAX;

	int bestScore = INT_MIN;
	std::list<Move*> bestMoves;
    
	auto moves = game->GetPossibleMoves();
	Player *player = game->GetCurrentState()->GetCurrentPlayer();

	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		Move *move = *it;
		GameState *subsequentState = move->Perform(false);

		int score;
		if (player->GetRelationship(subsequentState->GetCurrentPlayer()) == Player::Enemy)
			score = -FindBestScore(subsequentState, -beta, -alpha, ply - 1);
		else
			score = FindBestScore(subsequentState, alpha, beta, ply - 1);

		move->Reverse(false);
		delete subsequentState;

		//printf("Move has a score of %i\n", bestScore);

        if (score > bestScore) {
            bestScore = score;
			bestMoves.clear();
			bestMoves.push_back(move);
        }
        else if (score == bestScore)
            bestMoves.push_back(move);
    }
	printf("Best %i move(s) have a score of %i\n", bestMoves.size(), bestScore);
	int selectedIndex = rand() % bestMoves.size(), index = 0; // this is NOT randomizing, at least in the emscripten
	
	for (auto it = bestMoves.begin(); it != bestMoves.end(); it++)
	{
		if (index == selectedIndex)
			return *it;

		index++;
	}
	
	return 0;
}


int AI_AlphaBeta::FindBestScore(GameState *state, int alpha, int beta, int depth)
{
	EndOfGame *endOfGame = game->GetEndOfGame();
	EndOfGame::CheckType_t gameEndType = endOfGame->CheckEndOfTurn(state);
	if (gameEndType != EndOfGame::None)
		return GetScoreForEndOfGame(gameEndType);

	if (depth == 0)
		return EvaluateBoard(state); // should be quiescing here

	auto moves = state->DeterminePossibleMoves();

	gameEndType = endOfGame->CheckStartOfTurn(state, !moves->empty());
	if (gameEndType != EndOfGame::None)
	{
		for (auto it = moves->begin(); it != moves->end(); it++)
			delete *it;
		delete moves;
		return GetScoreForEndOfGame(gameEndType);
	}
	
	int bestScore = INT_MIN;
	Player *movePlayer = state->GetCurrentPlayer();

	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		Move *move = *it;
		GameState *subsequentState = move->Perform(false);
		int score;

		if (movePlayer->GetRelationship(subsequentState->GetCurrentPlayer()) == Player::Enemy)
			score = -FindBestScore(subsequentState, -beta, -alpha, depth - 1);
		else
			score = FindBestScore(subsequentState, alpha, beta, depth - 1);

		move->Reverse(false);
		if (score >= beta)
		{
			bestScore = score;

			for (; it != moves->end(); it++)
				delete *it;
			break;
		}
		else if(score > bestScore)
		{
			bestScore = score;
			if (score > alpha)
				alpha = score;
		}
		delete move;
	}

	//printf("FindBestScore at depth %i returns %i\n", depth, alpha);

	delete moves;
	return bestScore;
}


int AI_AlphaBeta::GetScoreForEndOfGame(EndOfGame::CheckType_t result)
{
	switch (result)
	{
	case EndOfGame::Win:
		return INT_MAX;
	case EndOfGame::Lose:
		return INT_MIN;
	case EndOfGame::Draw:
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