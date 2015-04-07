#include "AI.h"
#include "Game.h"
#include "GameState.h"
#include "Move.h"
#include "Piece.h"
#include "PieceType.h"
#include "Player.h"
#include <float.h>


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

	ReportError("Random AI failed to select a move\n");
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

	ReportError("Random Capture AI failed to select a move\n");
	return 0;
}


Move *AI_AlphaBeta::SelectMove()
{
	double alpha = DBL_MIN;
	double beta = DBL_MAX;

    double bestScore = DBL_MIN;
	std::list<Move*> bestMoves;
    
	auto moves = game->GetPossibleMoves();

	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		Move *move = *it;
        double score = GetMoveScore(move, alpha, beta, ply);

        if (score > bestScore) {
            bestScore = score;
			bestMoves.clear();
			bestMoves.push_back(move);
        }
        else if (score == bestScore)
            bestMoves.push_back(move);
    }

	int selectedIndex = rand() % moves->size(), index = 0;
	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		if (index == selectedIndex)
			return *it;

		index++;
	}

	ReportError("Alpha-Beta AI failed to select a move\n");
	return 0;
}


double AI_AlphaBeta::FindBestScore(GameState *state, double alpha, double beta, int depth)
{
	
	if (depth == 0)
        return EvaluateBoard(state);
    
	bool anyMoves = false;
    auto moves = state->DeterminePossibleMoves();

	for (auto it = moves->begin(); it != moves->end(); it++)
    {
		Move *move = *it;
        if (!anyMoves)
		{
            anyMoves = true;

			EndOfGame::CheckType_t gameEnd = game->GetEndOfGame()->CheckStartOfTurn(state, anyMoves);
			if (gameEnd != EndOfGame::None)
			{
				for (; it != moves->end(); it++)
					delete *it;
				delete moves;

				return GetScoreForEndOfGame(gameEnd);
			}
        }

        double score = GetMoveScore(move, alpha, beta, depth);
		delete move;

		if (score >= beta)
		{
			for (it++; it != moves->end(); it++)
				delete *it;
			delete moves;

			return beta;
		}
        if (score > alpha)
            alpha = score;
    }

	delete moves;

    if (!anyMoves)
        return GetScoreForEndOfGame(game->GetEndOfGame()->CheckStartOfTurn(state, anyMoves));

    return alpha;
}


double AI_AlphaBeta::GetMoveScore(Move *move, double alpha, double beta, int depth)
{
	GameState *subsequentState = move->Perform(false);

	EndOfGame::CheckType_t gameEnd = game->GetEndOfGame()->CheckEndOfTurn(move->GetPrevState(), move);
	double score;

	if (gameEnd == EndOfGame::None)
	{
        if (move->GetPlayer()->GetRelationship(subsequentState->GetCurrentPlayer()) == Player::Enemy)
            score = -FindBestScore(subsequentState, -beta, -alpha, depth - 1);
        else
            score = FindBestScore(subsequentState, alpha, beta, depth - 1);
    }
	else
		score = GetScoreForEndOfGame(gameEnd);

    move->Reverse(false);
	delete subsequentState;
    return score;
}


double AI_AlphaBeta::GetScoreForEndOfGame(EndOfGame::CheckType_t result)
{
	switch (result)
	{
	case EndOfGame::Win:
		return DBL_MAX;
	case EndOfGame::Lose:
		return DBL_MIN;
	case EndOfGame::Draw:
	default:
		return 0;
	}
}


double AI_AlphaBeta::EvaluateBoard(GameState *state)
{
	double score = 0;

	Player *current = state->GetCurrentPlayer();

	auto players = game->GetPlayers();
	for (auto itPlayers = players.begin(); itPlayers != players.end(); itPlayers++)
    {
		Player *other = *itPlayers;
        double playerScore = 0;

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