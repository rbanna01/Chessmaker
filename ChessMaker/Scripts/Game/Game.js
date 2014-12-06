function Game() {
    this.board = null;
    this.players = [];
    this.currentPlayer = null;
    this.turnOrder = null;
    this.endOfGame = null;
    this.moveNumber = 1;
    this.showCaptured = true;
    this.showHeld = false;

    this.holdCapturedPieces = false;
}

Game.parse = function (xml, boardRootElement) {
    xml = xml.firstChild;

    var game = new Game();
    game.board = new Board(game);

    var defs = SVG('defs'); var boardSVG;
    var boardXml = xml.firstChild;
    if (boardXml.nodeName == 'board')
        boardSVG = game.board.loadSVG(boardXml, defs);
    else
        throw 'can\'t find "board" node in game definition';

    var dirsXml = boardXml.nextSibling;
    if (dirsXml != null && dirsXml.nodeName == 'dirs')
        game.board.parseDirections(dirsXml);
    else {
        dirsXml = boardXml;
        console.log('can\'t find "dirs" node in game definition');
    }

    var piecesXml = dirsXml.nextSibling;
    if (piecesXml != null && piecesXml.nodeName == 'pieces')
        PieceType.parseAll(piecesXml, defs);
    else {
        piecesXml = dirsXml;
        console.log('can\'t find "pieces" node in game definition');
    }

    var setupXml = piecesXml.nextSibling;
    if (setupXml != null && setupXml.nodeName == 'setup')
        Player.parseAll(setupXml, game, boardSVG);
    else {
        setupXml = piecesXml;
        console.log('can\'t find "setup" node in game definition');
    }

    var rulesXml = setupXml.nextSibling;
    if (rulesXml != null && rulesXml.nodeName == 'rules')
        game.parseRules(rulesXml);
    else {
        game.parseRules(null);
        rulesXml = setupXml;
        console.log('can\'t find "rules" node in game definition');
    }

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

    boardRootElement.appendChild(boardSVG);
    return game;
};

Game.prototype.parseRules = function (xml) {
    var turnOrder = xml == null ? null : xml.firstChild;
    var endOfGame = null;
    if (turnOrder != null && turnOrder.nodeName == 'turnOrder') {
        this.turnOrder = TurnOrder.parse(turnOrder, this);
        endOfGame = turnOrder.nextSibling;
    }
    else {
        this.turnOrder = TurnOrder.createDefault(this);

        if (turnOrder != null)
            endOfGame = turnOrder;
        console.log('can\'t find "turnOrder" node within "rules" section of game definition');
    }

    if (endOfGame != null && endOfGame.nodeName == 'endOfGame') {
        this.endOfGame = EndOfGame.parse(endOfGame, this);
    }
    else {
        this.endOfGame = EndOfGame.createDefault(this);
        console.log('can\'t find "endOfGame" node within "rules" section of game definition');
    }
};

Game.prototype.endTurn = function () {
    for (var i = 0; i < this.currentPlayer.piecesOnBoard.length; i++)
        this.currentPlayer.piecesOnBoard[i].cachedMoves = null;

    var result = this.endOfGame.checkEndOfTurn();
    if (result !== undefined) {
        this.processEndOfGame(result);
        return;
    }

    this.startNextTurn();
};

Game.prototype.startNextTurn = function () {
    this.currentPlayer = this.turnOrder.getNextPlayer();

    $('#nextMove').text(this.currentPlayer.name.substr(0, 1).toUpperCase() + this.currentPlayer.name.substr(1) + ' to move');

    var anyPossibleMoves = this.ensureUniqueMoveNotation();
    var result = this.endOfGame.checkStartOfTurn(anyPossibleMoves);
    if (result !== undefined) {
        this.processEndOfGame(result);
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

Game.prototype.processEndOfGame = function (result) {
    if (result == EndOfGame.Type.Win)
        this.endGame(this.currentPlayer);
    else if (result == EndOfGame.Type.Draw)
        this.endGame(null);
    else if (result == EndOfGame.Type.Lose) {
        if (game.players.length == 2) {
            var other = game.players[0];
            if (other == game.currentPlayer)
                other = game.players[1];
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