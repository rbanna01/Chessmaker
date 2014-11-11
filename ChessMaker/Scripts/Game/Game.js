function Game() {
    this.board = null;
    this.players = {};
    this.currentPlayer = null;
    this.moveNumber = 1;
}

Game.prototype.setupTurnOrder = function () {
    // set up a cycle between the players. At some point, this is presumably gonna need to be more complex.
    var prevPlayer = null, firstPlayer = null;
    for (var name in this.players) {
        var player = this.players[name];
        if (prevPlayer == null)
            firstPlayer = player;
        else
            prevPlayer.nextPlayer = player;

        prevPlayer = player;
    }
    prevPlayer.nextPlayer = firstPlayer;
    this.currentPlayer = firstPlayer;
};

Game.prototype.endTurn = function () {
    for (var j = 0; j < this.currentPlayer.piecesOnBoard.length; j++)
        this.currentPlayer.piecesOnBoard[j].clearPossibleMoves();

    this.currentPlayer = this.currentPlayer.nextPlayer;
};