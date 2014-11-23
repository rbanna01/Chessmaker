function AI_Random() {

}

// Picks from all available moves, completely randomly.
AI_Random.prototype.selectMove = function () {
    var player = game.currentPlayer;

    var allMoves = [];
    for (var i = 0; i < player.piecesOnBoard.length; i++) {
        var piece = player.piecesOnBoard[i];
        allMoves = allMoves.concat(piece.cachedMoves);
    }

    var index = Math.floor(Math.random() * allMoves.length);
    return allMoves[index];
};

var AI = new AI_Random();