#include "Game.h"
#include "Board.h"
#include "EndOfGame.h"
#include "GameState.h"
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
		// todo: report error, game has already started
		return;
	}

	currentState = new GameState(this, turnOrder->GetNextPlayer(), 1);
	StartNextTurn();
}


void Game::StartNextTurn()
{
#ifndef NO_SVG
	//$('#nextMove').text(this.state.currentPlayer.name.substr(0, 1).toUpperCase() + this.state.currentPlayer.name.substr(1) + ' to move');
	// todo: implement this
#endif

	bool anyPossibleMoves = currentState->PrepareMovesForTurn();
	EndOfGame::CheckType_t result = endOfGame->CheckStartOfTurn(currentState, anyPossibleMoves);
	if (result != EndOfGame::None)
	{
		ProcessEndOfGame(result);
		return;
	}

	if (currentState->currentPlayer->type != Player::Local)
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
#ifndef NO_SVG
		//$('#wait').show();
		// todo: implement this
	}
	else
	{
		//$('#wait').hide();
		// todo: implement this
#endif
	}
}


void Game::EndTurn(GameState *newState, Move *move)
{
	EndOfGame::CheckType_t result = endOfGame->CheckEndOfTurn(currentState, move);
	if (result != EndOfGame::None) {
		ProcessEndOfGame(result);
		return;
	}

	currentState = newState;
	StartNextTurn();
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
		if (players.size() == 2) {
			Player *other = players.front();
			if (other == currentState->currentPlayer)
				other = players.back();
			EndGame(other);
		}
		else {
			// the current player should be removed from the game. If none remain, game is drawn. If one remains, they win. Otherwise, it continues.
			// todo: report error, can't (yet) handle a player losing in a game that doesn't have two players.
		}
		break;
	}
}


void Game::EndGame(Player *victor)
{
	/*var text;
	if (victor == null)
		text = 'Game finished, stalemate';
	else
		text = 'Game finished, ' + victor.name + ' wins';
	$('#nextMove').text(text);
	$('#wait').hide();*/
	// todo: implement this
}