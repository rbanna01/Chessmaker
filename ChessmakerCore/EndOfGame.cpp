#include "EndOfGame.h"

EndOfGame::EndOfGame()
{
	illegalMovesSpecified = false;
}


EndOfGame::~EndOfGame()
{
	while (!startOfTurnChecks.empty())
		delete startOfTurnChecks.front(), startOfTurnChecks.pop_front();

	while (!endOfTurnChecks.empty())
		delete endOfTurnChecks.front(), endOfTurnChecks.pop_front();
}


EndOfGame *EndOfGame::CreateDefault()
{
	/*var endOfGame = new EndOfGame();

    var condition = new EndOfGameConditions();
    // add a <pieceCount owner="self" check="equals">0</pieceCount> condition here
    var outOfPieces = new EndOfGameCheck(EndOfGame.Type.Lose, condition);
    endOfGame.startOfTurnChecks.push(outOfPieces);

    condition = new EndOfGameConditions();
    condition.elements.push[new EndOfGameConditions_cannotMove()];
    var outOfMoves = new EndOfGameCheck(EndOfGame.Type.Draw, condition);
    endOfGame.startOfTurnChecks.push(outOfMoves);

    return endOfGame;*/

	// todo: implement this
	return 0;
}


EndOfGame::CheckType_t EndOfGame::CheckStartOfTurn(GameState *state, bool canMove)
{
	/*// return Win/Lose/Draw, or undefined if the game isn't over yet
    for (var i = 0; i < this.startOfTurnChecks.length; i++) {
        var check = this.startOfTurnChecks[i];
        if (check.conditions.isSatisfied(state, anyPossibleMoves))
            return check.type;
    }

    if (anyPossibleMoves)
        return undefined;

    if (state.currentPlayer.piecesOnBoard.length == 0)
        return EndOfGame::Lose; // can't move and have no pieces. lose.

	return EndOfGame::Draw; // can't move, but have pieces. draw.
	*/

    return EndOfGame::Draw; // todo: implement this
}


EndOfGame::CheckType_t EndOfGame::CheckEndOfTurn(GameState *state, Move *move)
{
	/*// return Win/Lose/Draw/IllegalMove, or undefined if the game isn't over yet

    var noNextPlayer = state.game.turnOrder.getNextPlayer() == null;
    state.game.turnOrder.stepBackward();

    for (var i = 0; i < this.endOfTurnChecks.length; i++) {
        var check = this.endOfTurnChecks[i];
        if (check.conditions.isSatisfied(state, true))
            return check.type;
    }

    // if the next player is null, we've reached the end of the turn order
    if (noNextPlayer)
        return Draw;

    return undefined;*/

	return Draw; // todo: implement this
}