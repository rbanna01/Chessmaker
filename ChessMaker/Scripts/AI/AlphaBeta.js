function AI_AlphaBeta(ply) {
    this.ply = ply;
}

AI_AlphaBeta.prototype.selectMove = function () {
    var player = game.currentPlayer;

    var alpha = Number.NEGATIVE_INFINITY;
    var beta = Number.POSITIVE_INFINITY;

    var bestScore = Number.NEGATIVE_INFINITY;
    var bestMoves = [];
    var pieces = player.piecesOnBoard.slice();
    for (var i = 0; i < pieces.length; i++) {
        var piece = pieces[i];
        var moves = piece.cachedMoves;
        for (var j = 0; j < moves.length; j++) {
            var move = moves[j];

            move.perform(game, false);
            game.moveNumber++;

            var score;
            if (player.getRelationship(player.nextPlayer) == Player.Relationship.Enemy)
                score = -this.findBestScore(player.nextPlayer, -beta, -alpha, this.ply - 1);
            else
                score = this.findBestScore(player.nextPlayer, alpha, beta, this.ply - 1);

            if (score > bestScore) {
                bestScore = score;
                bestMoves = [move];
            }
            else if (score == bestScore)
                bestMoves.push(move);

            game.moveNumber--;
            move.reverse(game, false);
        }
    }

    var i = Math.floor(Math.random() * bestMoves.length);
    return bestMoves[i];
};

AI_AlphaBeta.prototype.findBestScore = function (player, alpha, beta, depth) {
    if (depth == 0) // or if this is the end of the game...
        return this.evaluateBoard(player);

    var anyMoves = false;
    var pieces = player.piecesOnBoard.slice();
    for (var i = 0; i < pieces.length; i++) {
        var piece = pieces[i];
        var moves = piece.determinePossibleMoves(game);

        for (var j = 0; j < moves.length; j++) {
            anyMoves = true;
            var move = moves[j];

            move.perform(game, false);
            game.moveNumber++;

            var victor = game.checkForEnd();
            if (victor !== undefined) {
                game.moveNumber--;
                move.reverse(game, false);
                if (victor == null)
                    return 0;
                return player.getRelationship(victor) == Player.Relationship.Enemy ? Number.NEGATIVE_INFINITY : Number.POSITIVE_INFINITY;
            }

            var score;
            if (player.getRelationship(player.nextPlayer) == Player.Relationship.Enemy)
                score = -this.findBestScore(player.nextPlayer, -beta, -alpha, depth - 1);
            else
                score = this.findBestScore(player.nextPlayer, alpha, beta, depth - 1);

            game.moveNumber--;

            if (score >= beta) {
                move.reverse(game, false);
                return beta;
            }
            if (score > alpha)
                alpha = score;

            move.reverse(game, false);
        }
    }

    if (!anyMoves)
        return 0; // stalemate, if nothing can move

    return alpha;
}

AI_AlphaBeta.prototype.evaluateBoard = function (player) {
    var score = 0;

    for (var i = 0; i < game.players.length; i++) {
        var other = game.players[i];
        var playerScore = 0;

        // add piece value to score
        for (var j = 0; j < other.piecesOnBoard.length; j++) {
            var piece = other.piecesOnBoard[j];
            playerScore += piece.value;

            // calculating possible moves (again?) here is gonna slow things down more

            // one simple option
            //playerScore += 0.1 * piece.determinePossibleMoves(game).length;

            // more complicated
            /*var moves = piece.determinePossibleMoves(game);
            for (var k = 0; k < moves.length; k++)
                playerScore += moves[k].isCapture() ? 0.2 : 0.1;*/
        }

        if (player.getRelationship(other) == Player.Relationship.Enemy)
            score -= playerScore;
        else
            score += playerScore;
    }
    return score;
};