// Picks from all available moves, completely randomly.
function AI_selectMove() {
    var player = game.currentPlayer;

    var allMoves = [];
    for (var i = 0; i < player.piecesOnBoard.length; i++) {
        var piece = player.piecesOnBoard[i];
        allMoves = allMoves.concat(piece.getPossibleMoves(game));
    }

    var index = Math.floor(Math.random() * allMoves.length);
    return allMoves[index];
}