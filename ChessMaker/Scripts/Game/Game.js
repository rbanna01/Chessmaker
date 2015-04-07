"use strict";

function Game() {
    this.showCaptured = true;
    this.showHeld = false;
}

Game.prototype.showMoveOptions = function (piece) {
    var moves = this.state.possibleMovesByPiece[piece.elementID];
    if (moves === undefined)
        return;
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

    var moves = this.state.possibleMovesByPiece[piece.elementID];
    for (var i = 0; i < moves.length; i++) {
        var move = moves[i];

        var destCell = move.getEndPos();
        if (destCell != cell)
            continue;

        this.performMove(move);
        break;
    }
};