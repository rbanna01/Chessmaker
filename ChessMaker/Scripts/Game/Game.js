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

Game.prototype.endTurn = function (newState, move) {
    var result = this.endOfGame.checkEndOfTurn(this.state, move);
    if (result !== undefined) {
        this.processEndOfGame(result);
        return;
    }

    this.state = newState;
    this.startNextTurn();
};

Game.prototype.startNextTurn = function () {
    $('#nextMove').text(this.state.currentPlayer.name.substr(0, 1).toUpperCase() + this.state.currentPlayer.name.substr(1) + ' to move');

    var anyPossibleMoves = this.state.prepareMovesForTurn();
    var result = this.endOfGame.checkStartOfTurn(this.state, anyPossibleMoves);
    if (result !== undefined) {
        this.processEndOfGame(result);
        return;
    }

    if (this.state.currentPlayer.type == Player.Type.Local)
        $('#wait').hide();
    else {
        $('#wait').show();

        if (this.state.currentPlayer.type == Player.Type.AI)
            setTimeout(function () {
                console.time('ai');
                var move = game.state.currentPlayer.AI.selectMove();
                console.timeEnd('ai');
                game.performMove(move);
            }, 1);
    }
};

Game.prototype.processEndOfGame = function (result) {
    if (result == EndOfGame.Type.Win)
        this.endGame(this.state.currentPlayer);
    else if (result == EndOfGame.Type.Draw)
        this.endGame(null);
    else if (result == EndOfGame.Type.Lose) {
        if (this.players.length == 2) {
            var other = this.players[0];
            if (other == this.state.currentPlayer)
                other = this.players[1];
            this.endGame(other);
        }
        else {
            // the current player should be removed from the game. If none remain, game is drawn. If one remains, they win. Otherwise, it continues.
            throw "Can't (yet) handle a player losing in a game that doesn't have two players.";
        }
    }
};

Game.prototype.endGame = function (victor) {
    var text;
    if (victor == null)
        text = 'Game finished, stalemate';
    else
        text = 'Game finished, ' + victor.name + ' wins';
    $('#nextMove').text(text);
    $('#wait').hide();
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