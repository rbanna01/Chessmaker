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
	GameEnd *result = CheckStartOfTurn(currentState, !possibleMoves->empty());
	startOfTurnAppendNotation = result->GetAppendNotation();

	if (result->GetType() != StateLogic::None)
	{
		ProcessEndOfGame(result);
		return false;
	}

	bool gotMessage = strlen(result->GetMessage()) != 0;
	bool local = currentState->GetCurrentPlayer()->GetType() == Player::Local;

	// capitalize first letter of player name
	std::string message = "";
	char c = toupper(currentState->GetCurrentPlayer()->GetName()[0]);
	message.insert(0, 1, c);
	message += currentState->GetCurrentPlayer()->GetName() + 1;
	message += " to move";

#ifdef EMSCRIPTEN
	EM_ASM_ARGS({startTurn($0, $1);}, message.c_str(), local);
	if (gotMessage)
		EM_ASM_ARGS({showMessage($0);}, result->GetMessage());
#endif

#ifdef CONSOLE
	printf("%s\n", message.c_str());
	if (gotMessage)
		printf("  %s\n", result->GetMessage());
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
	if (subsequentState == 0)
		return MoveError;

	if (EndTurn(subsequentState, move))
		return MoveComplete;
	else
		return GameComplete;
}

bool Game::EndTurn(GameState *newState, Move *lastMove)
{
	GameEnd *result = CheckEndOfTurn(currentState);
	
	// store the turn number for logging purposes
	int turnNumber = currentState->GetTurnNumber();
	ClearPossibleMoves(lastMove);
	delete currentState;

	if (result->GetType() != StateLogic::None) {
		ProcessEndOfGame(result);
		LogMove(lastMove, turnNumber, result->GetAppendNotation());
		delete lastMove;
		return false;
	}

	bool gotMessage = strlen(result->GetMessage()) != 0;

#ifdef EMSCRIPTEN
	if (gotMessage)
		EM_ASM_ARGS({ showMessage($0); }, result->GetMessage());
#endif

#ifdef CONSOLE
	if (gotMessage)
		printf("%s\n", result->GetMessage());
#endif

	currentState = newState;
	bool shouldContinue = StartNextTurn();

	// logging must happen AFTER the next StartNextTurn, so(e.g.) the * from a Check move can be applied to the previous turn's notation
	LogMove(lastMove, turnNumber, result->GetAppendNotation());
	delete lastMove;

	return shouldContinue;
}


GameEnd* Game::CheckStartOfTurn(GameState *state, bool canMove)
{
	return startOfTurnLogic->Evaluate(state, canMove);
}

GameEnd* Game::CheckEndOfTurn(GameState *state)
{
	return endOfTurnLogic->Evaluate(state, true);
}

void Game::ProcessEndOfGame(GameEnd *result)
{
	switch (result->GetType())
	{
	case StateLogic::Win:
		EndGame(currentState->currentPlayer, result->GetMessage());
		break;
	case StateLogic::Draw:
		EndGame(0, result->GetMessage());
		break;
	case StateLogic::Lose:
		if (players.size() == 2)
		{
			Player *other = players.front();
			if (other == currentState->currentPlayer)
				other = players.back();
			EndGame(other, result->GetMessage());
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


void Game::EndGame(Player *victor, const char *message)
{
	// capitalize first letter
	char c = toupper(message[0]);
	std::string output = "";
	output.insert(0, 1, c);
	output += message + 1;

	if (victor == 0)
	{
		output += ", game drawn\n";
	}
	else
	{
		output += ", ";
		output += victor->GetName();
		output += " wins\n";
	}

#ifdef CONSOLE
	printf(output.c_str());
#endif
#ifdef EMSCRIPTEN
	EM_ASM_ARGS({showGameEnd($0);}, output.c_str());
#endif
}

void Game::LogMove(Move *move, int turnNumber, const char *appendNotation)
{
	std::string notation = "";

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
	notation += startOfTurnAppendNotation;
	notation += appendNotation;

#ifdef CONSOLE
	printf("%s\n", notation.c_str());
#endif
	
#ifdef EMSCRIPTEN
	// as well as logging notation, this passes indicators for the cells that were moved from/to
	Cell *start = move->GetStartPos();
	Cell *end = move->GetEndPos();
	EM_ASM_ARGS({ logMove($0, $1, $2, $3, $4); }, move->GetPlayer()->GetName(), move->GetPrevState()->GetTurnNumber(), notation.c_str(), start == 0 ? "" : start->GetName(), end == 0 ? "" : end->GetName());
#endif
}

void Game::ClearPossibleMoves(Move *dontDelete)
{
	if (possibleMoves == 0)
		return;

	for (auto it = possibleMoves->begin(); it != possibleMoves->end(); it++)
		if (*it != dontDelete)
			delete *it;

	delete possibleMoves;
	possibleMoves = 0;
}