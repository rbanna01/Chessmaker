function AI_Negamax(ply) {
    this.ply = ply;
}

AI_Negamax.prototype.selectMove = function () {
    return this.findBestScore(game.currentPlayer, this.ply, true);
};

AI_Negamax.prototype.findBestScore = function (player, depth, returnMove) {
    if (depth == 0) // or if this is the end of the game...
        return this.evaluateBoard(player);

    var bestScore = Number.NEGATIVE_INFINITY;
    var bestMove = null;
    for (var i = 0; i < player.piecesOnBoard.length; i++) {
        var piece = player.piecesOnBoard[i];
        var moves = piece.getPossibleMoves(game);
        for (var j = 0; j < moves.length; j++) {
            var move = moves[j];

            move.perform(game, false);

            var score = this.findBestScore(player.nextPlayer, depth - 1, false);
            if (player.getRelationship(player.nextPlayer) == Player.Relationship.Enemy)
                score = -score;

            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }

            move.reverse(game, false);
        }
    }

    if (returnMove)
        return bestMove;

    return bestScore;
}

AI_Negamax.prototype.evaluateBoard = function (player) {
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
AI_Negamax.prototype.countCaptures = function (player) {
    var captures = 0;
    for (var i = 0; i < player.piecesOnBoard.length; i++) {
        var piece = player.piecesOnBoard[i];
        var moves = piece.getPossibleMoves(game);
        for (var j = 0; j < moves.length; j++)
            if (moves[j].isCapture())
                captures++;
        piece.clearPossibleMoves();
    }
    return captures;
};
*/
var AI = new AI_Negamax(4);