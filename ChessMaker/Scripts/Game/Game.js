function Game() {
    this.board = null;
    this.players = [];
    this.currentPlayer = null;
    this.moveNumber = 1;
    this.showCaptured = true;
    this.showHeld = false;

    this.holdCapturedPieces = false;
}

Game.prototype.setupTurnOrder = function () {
    // set up a cycle between the players. At some point, this is presumably gonna need to be more complex.
    var prevPlayer = null, firstPlayer = null;
    var listX = 25, listXStep = 50; // stepListX should be calculated based on the pixel width of ... either of the two containers. Probably. And whether or not we have multiple columns.

    for (var i = 0; i < this.players.length; i++) {
        var player = this.players[i];
        if (prevPlayer == null)
            firstPlayer = player;
        else
            prevPlayer.nextPlayer = player;

        player.pieceListX = listX;
        listX += listXStep;

        prevPlayer = player;
    }
    prevPlayer.nextPlayer = firstPlayer;
    this.currentPlayer = firstPlayer;
};

Game.prototype.checkForEnd = function () {
    // return the victor, or null if nobody wins. return undefined if the game isn't over yet.

    // for now, just count remaining pieces. Eventually, should look at victory conditions.
    var playerWithPieces = null;
    for (var i = 0; i < this.players.length; i++) {
        var player = this.players[i];
        if (player.piecesOnBoard.length == 0)
            continue;
        else if (playerWithPieces == null)
            playerWithPieces = player;
        else
            return undefined; // multiple players still have pieces, game is not over.
    }

    // return null if nobody has pieces left: stalemate. Otherwise, only one player has pieces left: they win.
    return playerWithPieces;
};

Game.prototype.endTurn = function (cannotMove) {
    for (var i = 0; i < this.currentPlayer.piecesOnBoard.length; i++)
        this.currentPlayer.piecesOnBoard[i].cachedMoves = null;

    var victor = this.checkForEnd();
    if (cannotMove !== undefined && victor === undefined)
        victor = null;
    if (victor !== undefined) {
        var text;
        if (victor == null)
            text = 'Game finished, stalemate';
        else
            text = 'Game finished, ' + victor.name + ' wins';
        $('#nextMove').text(text);
        $('#wait').hide();
        return;
    }

    this.moveNumber++;

    this.currentPlayer = this.currentPlayer.nextPlayer;
    this.startNextTurn();
};

Game.prototype.startNextTurn = function () {
    $('#nextMove').text(this.currentPlayer.name.substr(0, 1).toUpperCase() + this.currentPlayer.name.substr(1) + ' to move');

    if (!this.ensureUniqueMoveNotation()) {
        this.endTurn(true); // no available moves (stalemate)
        return;
    }

    if (this.currentPlayer.type == Player.Type.Local)
        $('#wait').hide();
    else {
        $('#wait').show();

        if (this.currentPlayer.type == Player.Type.AI)
            setTimeout(function () {
                console.time('ai');
                var move = game.currentPlayer.AI.selectMove();
                console.timeEnd('ai');
                game.performMove(move);
            }, 1);
    }
};

Game.prototype.ensureUniqueMoveNotation = function () {
    // calculate all moves, which pieces store internally. this just ensures they all have unique notation, whereas doing it one piece at a time wouldn't.
    var allMoves = {};
    var anyMoves = false;

    var pieces = this.currentPlayer.piecesOnBoard.slice();
    for (var i = 0; i < pieces.length; i++) {
        var piece = pieces[i];
        var movesForThisPiece = piece.determinePossibleMoves(this);
        piece.cachedMoves = movesForThisPiece;

        for (var j = 0; j < movesForThisPiece.length; j++) {
            var move = movesForThisPiece[j];

            for (var detailLevel = 1; detailLevel <= Move.maxNotationDetail; detailLevel++) {
                anyMoves = true;
                var notation = move.determineNotation(detailLevel);

                if (allMoves.hasOwnProperty(notation)) {
                    // another move uses this notation. Calculate a new, more-specific notation for that other move.
                    var other = allMoves[notation];
                    var otherNot = other.determineNotation(detailLevel + 1);
                    allMoves[otherNot] = other;
                }
                else {
                    allMoves[notation] = move;
                    break;
                }
            }
        }
    }
    return anyMoves;
};

Game.prototype.showMoveOptions = function (piece) {
    var moves = piece.cachedMoves;
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

    var moves = piece.cachedMoves;
    for (var i = 0; i < moves.length; i++) {
        var move = moves[i]
        var destCell = move.getEndPos();
        if (destCell != cell)
            continue;

        this.performMove(move);
        break;
    }
};

Game.prototype.performMove = function (move) {
    if (move.perform(this, true)) {
        this.logMove(this.currentPlayer, move);
        this.endTurn();
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