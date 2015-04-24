#include "Game.h"
#include "ai.h"
#include "Board.h"
#include "GameState.h"
#include "Move.h"
#include "Piece.h"
#include "StateLogic.h"
#include "TurnOrder.h"


Game::Game()
{
	board = 0;
	currentState = 0;
	startOfTurnLogic = 0;
	endOfTurnLogic = 0;
	turnOrder = 0;

	illegalMovesSpecified = false;
}


Game::~Game()
{
	if (board != 0)
		delete board;
	if (currentState != 0)
		delete currentState;
	if (startOfTurnLogic != 0)
		delete startOfTurnLogic;
	if (endOfTurnLogic != 0)
		delete endOfTurnLogic;
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
	StateLogic::GameEnd_t result = CheckStartOfTurn(currentState, !possibleMoves->empty());
	if (result != StateLogic::None)
	{
		ProcessEndOfGame(result);
		return false;
	}

	bool local = currentState->GetCurrentPlayer()->GetType() == Player::Local;

#ifdef EMSCRIPTEN
	EM_ASM_ARGS({setCurrentPlayer($0, $1);}, currentState->GetCurrentPlayer()->GetName(), local);
#endif

#ifdef CONSOLE
	printf("%s to move\n", currentState->GetCurrentPlayer()->GetName());
#endif

	if (local) // list possible moves
	{
#ifdef EMSCRIPTEN
		for (auto it = possibleMoves->begin(); it != possibleMoves->end(); it++)
		{
			Move *move = *it;
			EM_ASM_ARGS({ addPossibleMove($0, $1, $2); }, move->GetNotation(), move->GetPiece()->GetPosition()->GetName(), move->GetEndPos()->GetName());
		}
#endif
#ifdef CONSOLE
		std::string output = "Possible moves:\n";
		for (auto it = possibleMoves->begin(); it != possibleMoves->end(); it++)
		{
			Move *move = *it;
			output += move->GetNotation();
			output += "\n";
		}
		printf(output.c_str());
#endif
	}
	else if (currentState->GetCurrentPlayer()->GetType() == Player::AI)
	{
#ifdef EMSCRIPTEN
		EM_ASM(console.time("ai"));
#endif
		auto move = currentState->GetCurrentPlayer()->GetAI()->SelectMove();
#ifdef EMSCRIPTEN
		EM_ASM(console.timeEnd("ai"));
#endif
		if (move == 0)
		{
			ReportError("AI failed to select a move\n");
			return false;
		}
		PerformMove(move);
	}

	return true;
}

Game::MoveResult_t Game::PerformMove(Move *move)
{
	GameState *subsequentState = move->Perform(true);
	if (subsequentState != 0)
	{
		LogMove(currentState->currentPlayer, move);
		if (EndTurn(subsequentState))
			return MoveComplete;
		else
			return GameComplete;
	}
	else
	{
		return MoveError;
	}
}

bool Game::EndTurn(GameState *newState)
{
	StateLogic::GameEnd_t result = CheckEndOfTurn(currentState);
	ClearPossibleMoves();
	delete currentState;

	if (result != StateLogic::None) {
		ProcessEndOfGame(result);
		return false;
	}

	currentState = newState;
	return StartNextTurn();
}


StateLogic::GameEnd_t Game::CheckStartOfTurn(GameState *state, bool canMove)
{
	return startOfTurnLogic->Evaluate(state, canMove);
}

StateLogic::GameEnd_t Game::CheckEndOfTurn(GameState *state)
{
	return endOfTurnLogic->Evaluate(state, true);
}

void Game::ProcessEndOfGame(StateLogic::GameEnd_t result)
{
	switch (result)
	{
	case StateLogic::Win:
		EndGame(currentState->currentPlayer);
		break;
	case StateLogic::Draw:
		EndGame(0);
		break;
	case StateLogic::Lose:
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
	EM_ASM_ARGS({showGameEnd($0);}, text);
#endif
}

void Game::LogMove(Player *player, Move *move)
{
	std::string notation = "";

	int turnNumber = move->GetPrevState()->GetTurnNumber();
	int numPlayers = players.size();
	int displayNumber = (turnNumber + numPlayers + 1) / numPlayers - 1;
	notation += std::to_string(displayNumber);

	int playerNum = turnNumber % numPlayers;
	if (playerNum == 1) // first player's move
		notation += ".  ";
	else if (playerNum == 0) // last player's move
		notation += "...";
	else
		notation += ".. ";

	notation += move->GetNotation();

#ifdef CONSOLE
	printf("%s\n", notation.c_str());
#endif
	
#ifdef EMSCRIPTEN
	// as well as logging notation, this passes indicators for the cells that were moved from/to
	Cell *start = move->GetStartPos();
	Cell *end = move->GetEndPos();
	EM_ASM_ARGS({ logMove($0, $1, $2, $3, $4); }, currentState->GetCurrentPlayer()->GetName(), move->GetPrevState()->GetTurnNumber(), notation.c_str(), start == 0 ? "" : start->GetName(), end == 0 ? "" : end->GetName());
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