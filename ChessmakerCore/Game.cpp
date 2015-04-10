#include "Game.h"
#include "Board.h"
#include "EndOfGame.h"
#include "GameState.h"
#include "Move.h"
#include "TurnOrder.h"


Game::Game()
{
	board = 0;
	currentState = 0;
	endOfGame = 0;
	turnOrder = 0;
}


Game::~Game()
{
	if (board != 0)
		delete board;
	if (currentState != 0)
		delete currentState;
	if (endOfGame != 0)
		delete endOfGame;
	if (turnOrder != 0)
		delete turnOrder;

	ClearPossibleMoves();

	auto it = players.begin();
	while (it != players.end())
	{
		delete (*it);
		it++;
	}

	auto it2 = allPieceTypes.begin();
	while (it2 != allPieceTypes.end())
	{
		delete (*it2);
		it2++;
	}
}

void Game::Start()
{
	if (currentState != 0)
	{
		ReportError("Cannot start game, it has already started\n");
		return;
	}

	currentState = new GameState(this, turnOrder->GetNextPlayer(), 1);
	StartNextTurn();
}


bool Game::StartNextTurn()
{
	possibleMoves = currentState->PrepareMovesForTurn();
	EndOfGame::CheckType_t result = endOfGame->CheckStartOfTurn(currentState, !possibleMoves->empty());
	if (result != EndOfGame::None)
	{
		ProcessEndOfGame(result);
		return false;
	}

	bool local = currentState->currentPlayer->type != Player::Local;
	if (!local)
	{
		if (currentState->currentPlayer->type == Player::AI)
		{
			/* todo: implement this
			setTimeout(function() {
				console.time('ai');
				var move = game.state.currentPlayer.AI.selectMove();
				console.timeEnd('ai');
				game.performMove(move);
			}, 1);*/
		}
	}

#ifdef EMSCRIPTEN
	EM_ASM_ARGS({setCurrentPlayer($0, $1);}, currentState->GetCurrentPlayer()->GetName(), local);
#endif

#ifdef CONSOLE
	printf("%s to move\n", currentState->GetCurrentPlayer()->GetName());
#endif

	return true;
}

Game::MoveResult_t Game::PerformMove(Move *move)
{
	GameState *subsequentState = move->Perform(true);
	if (subsequentState != 0)
	{
		LogMove(currentState->currentPlayer, move);
		if (EndTurn(subsequentState, move))
			return MoveComplete;
		else
			return GameComplete;
	}
	else
	{
		return MoveError;
	}
}

bool Game::EndTurn(GameState *newState, Move *move)
{
	EndOfGame::CheckType_t result = endOfGame->CheckEndOfTurn(currentState, move);
	ClearPossibleMoves();
	delete currentState;

	if (result != EndOfGame::None) {
		ProcessEndOfGame(result);
		return false;
	}

	currentState = newState;
	return StartNextTurn();
}


void Game::ProcessEndOfGame(EndOfGame::CheckType_t result)
{
	switch (result)
	{
	case EndOfGame::Win:
		EndGame(currentState->currentPlayer);
		break;
	case EndOfGame::Draw:
		EndGame(0);
		break;
	case EndOfGame::Lose:
		if (players.size() == 2)
		{
			Player *other = players.front();
			if (other == currentState->currentPlayer)
				other = players.back();
			EndGame(other);
		}
		else
		{
			// the current player should be removed from the game. If none remain, game is drawn. If one remains, they win. Otherwise, it continues.
			// todo: implement this, somehow
			ReportError("Unfortunately, chessmaker can't (yet) handle a player losing in a game that doesn't have two players.\n");
		}
		break;
	default:
		break;
	}
}


void Game::EndGame(Player *victor)
{
	char text[PLAYER_NAME_LENGTH + 32];
	if (victor == 0)
		sprintf(text, "Game finished, stalemate\n");
	else
		sprintf(text, "Game finished, %s wins\n", victor->GetName());

#ifdef CONSOLE
	printf(text);
#endif
#ifdef EMSCRIPTEN
	EM_ASM_ARGS({setCurrentPlayer($0, true);}, text);
#endif
}

void Game::LogMove(Player *player, Move *move)
{
#ifdef CONSOLE
	printf("%s\n", move->GetNotation());
#endif
	
#ifdef EMSCRIPTEN
	EM_ASM_ARGS({ logMove($0, $1, $2); }, currentState->GetCurrentPlayer()->GetName(), move->GetPrevState()->GetTurnNumber(), move->GetNotation());
#endif
}

void Game::ClearPossibleMoves()
{
	if (possibleMoves == 0)
		return;

	while (!possibleMoves->empty())
		delete possibleMoves->front(), possibleMoves->pop_front();
	delete possibleMoves;
	possibleMoves = 0;
}