﻿function Game() {
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

Game.prototype.showMoveOptions = function (piece) {
    var moves = piece.getPossibleMoves(this);
    for (var i = 0; i < moves.length; i++) {
        var destCell = moves[i].getEndPos();
        var img = destCell.getImage();
        addClassSingle(img, 'option');
    }
};

Game.prototype.selectMoveByCell = function (piece, cell) {
    if (piece.ownerPlayer == null)
        console.log('piece.ownerPlayer is null');
    if (piece.ownerPlayer === undefined)
        console.log('piece.ownerPlayer is undefined');

    var moves = piece.getPossibleMoves(this);
    for (var i = 0; i < moves.length; i++) {
        var move = moves[i]
        var destCell = move.getEndPos();
        if (destCell != cell)
            continue;

        if (move.perform(this, true))
            this.endTurn();
        else
            console.log('unable to perform move');
        break;
    }
};