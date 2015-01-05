function GameState(game, prevMove) {
    this.game = game;
    this.move = prevMove;

    this.possibleMoves = null;
    this.possibleMovesByPiece = null;
    this.threatOnlyMoves = []; // these are never actually performed or followed up on, but are used in calculating whether moves would be disallowed due to check. e.g. the king's forward "attack" in xiangqui.
}

GameState.prototype.determinePossibleMoves = function (player) {
    if (this.possibleMoves === null) {
        this.possibleMoves = [];

        var pieces = player.piecesOnBoard.slice();
        for (var i = 0; i < pieces.length; i++) {
            var moves = this.determineMovesForPiece(pieces[i]);

            for (var j = 0; j < moves.length; j++) {
                var move = moves[j];
                /*if (move.isThreatOnly)
                    this.threatOnlyMoves.push(move);
                else*/
                    this.possibleMoves.push(move);
            }
        }

        // also look at held pieces?
    }
    return this.possibleMoves;
};

GameState.prototype.determineMovesForPiece = function (piece) {
    var moves = [];

    if (piece.pieceState == Piece.State.OnBoard) {
        var moveTemplate = new Move(piece.ownerPlayer, piece, piece.position, this.game.moveNumber);

        /*// get promotion possibilities
        piece.pieceType.promotionOpportunities.each(function (op) {
            if (!op.mandatory && op.type != PromotionType.EndOfMove && op.isAvailable(moveTemplate, this.game))
                op.options.each(function (option) {
                    piece.possibleMoves.push(new Promotion(piece.ownerPlayer, piece, option, this.game.moveNumber, op.type == PromotionType.CountsAsMove));
                });
        });
        */
        // and then get move possibilities
        for (var i = 0; i < piece.pieceType.moves.length; i++) {
            var move = piece.pieceType.moves[i];
            var possibilities = move.appendValidNextSteps(moveTemplate, piece, this.game, null);
            for (var j = 0; j < possibilities.length; j++)
                moves.push(possibilities[j]);
        }
    }/*
    else if (piece.pieceState == Piece.State.Held) {
        this.game.board.cells.items.each(function (coord, cell) {
            if (cell.value != Board.CellType.Normal)
                return;

            var move = new Move(piece.ownerPlayer, piece, null, this.game.moveNumber, true);
            move.addStep(MoveStep.CreateDrop(piece, coord, piece.ownerPlayer));

            if (this.game.rules.dropPiecesWhen == null || this.game.rules.dropPiecesWhen.isSatisfied(move, this.game))
                piece.possibleMoves.push(move);
        });
    }*/

    return moves;
}

// sort possible states (moves) by piece, determine notation for each move (done all at once to ensure they are unique), and return whether any moves are possible or not
GameState.prototype.prepareMovesForTurn = function () {
    var allMoves = {};
    var anyMoves = false;
    this.possibleMovesByPiece = {};

    if (this.possibleMoves === null)
        this.determinePossibleMoves(this.game.currentPlayer);

    for (var i = 0; i < this.possibleMoves.length; i++) {
        anyMoves = true;
        var move = this.possibleMoves[i];

        // sort moves by piece, for ease of access in the UI
        if (!this.possibleMovesByPiece.hasOwnProperty(move.piece.elementID))
            this.possibleMovesByPiece[move.piece.elementID] = [move];
        else
            this.possibleMovesByPiece[move.piece.elementID].push(move);

        // ensure unique notation
        for (var detailLevel = 1; detailLevel <= Move.maxNotationDetail; detailLevel++) {
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
    return anyMoves;
};