function AI_Random() {

}

// Picks from all available moves, completely randomly.
AI_Random.prototype.selectMove = function () {
    var allMoves = game.state.possibleMoves;
    var index = Math.floor(Math.random() * allMoves.length);
    return allMoves[index];
};