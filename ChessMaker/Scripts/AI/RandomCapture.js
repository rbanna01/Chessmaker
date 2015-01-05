function AI_RandomCapture() {

}

// Picks from all available CAPTURE moves, completely randomly.
// If there are none, picks from all available moves, completely randomly.
AI_RandomCapture.prototype.selectMove = function () {
    var allMoves = game.state.possibleMoves;

    var captures = [];
    for (var i = 0; i < allMoves.length; i++) {
        var move = allMoves[i];
        if (move.isCapture())
            captures.push(move);
    }

    if (captures.length > 0) {
        var index = Math.floor(Math.random() * captures.length);
        return captures[index];
    }

    var index = Math.floor(Math.random() * allMoves.length);
    return allMoves[index];
};