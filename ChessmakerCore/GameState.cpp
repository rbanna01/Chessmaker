#include "GameState.h"
#include "Game.h"

GameState::GameState(Game *game, int turnNumber)
{
	this->game = game;
	this->turnNumber = turnNumber;
}


GameState::~GameState()
{
	
}


// sort possible states (moves) by piece, determine notation for each move (done all at once to ensure they are unique), and return whether any moves are possible or not
bool GameState::PrepareMovesForTurn()
{
	/* todo: implement this
	var allMoves = {};
    bool anyMoves = false;
    this.possibleMovesByPiece = {};

    var moves = this.determinePossibleMoves();

    for (var i = 0; i < moves.length; i++) {
        anyMoves = true;
        var move = moves[i];

        // sort moves by piece, for ease of access in the UI
        if (!this.possibleMovesByPiece.hasOwnProperty(move.piece.elementID))
            this.possibleMovesByPiece[move.piece.elementID] = [move];
        else
            this.possibleMovesByPiece[move.piece.elementID].push(move);

        // ensure unique notation
        for (var detailLevel = 1; detailLevel <= Move.maxNotationDetail; detailLevel++) {
            var notation = move.determineNotation(detailLevel);

            if (allMoves.hasOwnProperty(notation)) {
                // another move uses this notation. Calculate a new, more-specific notation for that other move.
                var other = allMoves[notation];
                var otherNot = other.determineNotation(detailLevel + 1);
                allMoves[otherNot] = other;
            }
            else {
                allMoves[notation] = move;
                break;
            }
        }
    }
    return anyMoves;*/
	return false;
}