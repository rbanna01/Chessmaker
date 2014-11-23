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
    // for now, just count the number of pieces
    var friendlyPieces = player.piecesOnBoard.length;
    var opponentPieces = 0;

    var other = player.nextPlayer;
    //while (other != player) {
        if (player.getRelationship(other) == Player.Relationship.Enemy)
            opponentPieces += other.piecesOnBoard.length;
        else
            friendlyPieces += other.piecesOnBoard.length;

        other = player.nextPlayer;
    //}
    
    return friendlyPieces - opponentPieces;

    /*var myCaptures = this.countCaptures(player);
    var opponentCaptures = 0;

    var other = player.nextPlayer;
    while (other != player)
    {
        opponentCaptures += this.countCaptures(other);
        other = player.nextPlayer;
    }

    return myCaptures - opponentCaptures;*/
};
/*
AI_AlphaBeta.prototype.countCaptures = function (player) {
    var captures = 0;
    for (var i = 0; i < player.piecesOnBoard.length; i++) {
        var piece = player.piecesOnBoard[i];
        var moves = piece.determinePossibleMoves(game);
        for (var j = 0; j < moves.length; j++)
            if (moves[j].isCapture())
                captures++;
    }
    return captures;
};
*/
var AI = new AI_AlphaBeta(4);