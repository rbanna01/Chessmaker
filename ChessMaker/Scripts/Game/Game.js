function Game() {
    this.board = null;
    this.players = {};
    this.currentPlayer = null;
    this.moveNumber = 1;
}

Game.prototype.moveToNextPlayer = function () {
    this.currentPlayer = this.currentPlayer.nextPlayer;
    return this.currentPlayer;
};

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