"use strict";

function Game() {
    this.board = null;
    this.players = [];
    this.turnOrder = null;
    this.state = null;
    this.endOfGame = null;
    this.showCaptured = true;
    this.showHeld = false;

    this.holdCapturedPieces = false;
}

Game.parse = function (xml, boardRootElement) {

    // this needs enhanced to also allow for remote players
    if (typeof AIs != 'undefined')
        for (var i = 0; i < game.players.length; i++) {
            var AI = AIs[i];
            if (AI == null)
                continue;

            var player = game.players[i];
            player.type = Player.Type.AI;
            player.AI = AIs[i];

            if (i >= AIs.length - 1)
                break;
        }

    return game;
};

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

Game.prototype.performMove = function (move) {
    if (move.perform(this, true)) {
        this.logMove(this.state.currentPlayer, move);
        this.endTurn(move.subsequentState, move);
    }
    else
        console.log('unable to perform move');
}

Game.prototype.logMove = function (player, move) {
    var historyDiv = $('#moveHistory');

    $('<div/>', {
        class: 'move ' + player.name,
        number: move.moveNumber,
        html: move.notation
    }).appendTo(historyDiv);

    historyDiv.get(0).scrollTop = historyDiv.get(0).scrollHeight;
};