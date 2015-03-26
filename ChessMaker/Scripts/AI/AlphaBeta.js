"use strict";

function AI_AlphaBeta(ply) {
    this.ply = ply;
}

AI_AlphaBeta.prototype.selectMove = function () {
    var alpha = Number.NEGATIVE_INFINITY;
    var beta = Number.POSITIVE_INFINITY;

    var bestScore = Number.NEGATIVE_INFINITY;
    var bestMoves = [];

    var moves = game.state.determinePossibleMoves();

    for (var i = 0; i < moves.length; i++) {
        var move = moves[i];
        var score = this.getMoveScore(move, alpha, beta, this.ply);

        if (score > bestScore) {
            bestScore = score;
            bestMoves = [move];
        }
        else if (score == bestScore)
            bestMoves.push(move);
    }

    var i = Math.floor(Math.random() * bestMoves.length);
    return bestMoves[i];
};

AI_AlphaBeta.prototype.findBestScore = function (state, alpha, beta, depth) {
    if (depth == 0)
        return this.evaluateBoard(state);

    var anyMoves = false;
    var moves = state.determinePossibleMoves();

    for (var i = 0; i < moves.length; i++) {
        if (!anyMoves) {
            anyMoves = true;

            var resultScore = this.getScoreForEndOfGame(game.endOfGame.checkStartOfTurn(state, anyMoves));
            if (resultScore !== undefined)
                return resultScore;
        }

        var score = this.getMoveScore(moves[i], alpha, beta, depth);

        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }

    if (!anyMoves)
        return this.getScoreForEndOfGame(game.endOfGame.checkStartOfTurn(state, anyMoves));

    return alpha;
}

AI_AlphaBeta.prototype.getMoveScore = function (move, alpha, beta, depth) {
    var subsequentState = move.perform(game, false);

    var score = this.getScoreForEndOfGame(game.endOfGame.checkEndOfTurn(move.prevState, move));
    
    if (score === undefined) {
        if (move.player.getRelationship(subsequentState.currentPlayer) == Player.Relationship.Enemy)
            score = -this.findBestScore(subsequentState, -beta, -alpha, depth - 1);
        else
            score = this.findBestScore(subsequentState, alpha, beta, depth - 1);
    }

    move.reverse(game, false);
    return score;
};

AI_AlphaBeta.prototype.getScoreForEndOfGame = function (result) {
    switch(result){
        case EndOfGame.Type.Win:
            return Number.POSITIVE_INFINITY;
        case EndOfGame.Type.Lose:
            return Number.NEGATIVE_INFINITY;
        case EndOfGame.Type.Draw:
            return 0;
        default:
            return undefined;
    }
};

AI_AlphaBeta.prototype.evaluateBoard = function (state) {
    var score = 0;

    for (var i = 0; i < game.players.length; i++) {
        var other = game.players[i];
        var playerScore = 0;

        // add piece value to score
        for (var j = 0; j < other.piecesOnBoard.length; j++) {
            var piece = other.piecesOnBoard[j];
            playerScore += piece.pieceType.value;

            // calculating possible moves (again?) here is gonna slow things down more

            // NOTE! Neither of these would be cached! they ought to be!

            // one simple option
            //playerScore += 0.1 * move.subsequentState.determineMovesForPiece(piece).length;

            // more complicated
            /*var moves = move.subsequentState.determineMovesForPiece(piece);
            for (var k = 0; k < moves.length; k++)
                playerScore += moves[k].isCapture() ? 0.2 : 0.1;*/
        }

        if (state.currentPlayer.getRelationship(other) == Player.Relationship.Enemy)
            score -= playerScore;
        else
            score += playerScore;
    }
    return score;
};