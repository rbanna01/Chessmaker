"use strict";

function GameState(game, prevMove, moveNum) {
    this.game = game;
    this.move = prevMove;
    this.currentPlayer = null;
    this.moveNumber = moveNum;

    this.possibleMovesByPiece = null;
}

GameState.prototype.determinePossibleMoves = function () {
    var moves = [];
    this.calculateMovesForPlayer(this.currentPlayer, moves);
    return moves;
};

GameState.prototype.determineThreatMoves = function () {
    var threatMoves = [];

    for (var p = 0; p < game.players.length; p++) {
        var opponent = game.players[p];

        if (this.currentPlayer.getRelationship(opponent) != Player.Relationship.Enemy)
            continue;

        this.calculateMovesForPlayer(opponent, threatMoves);
    }

    return threatMoves;
};

GameState.prototype.calculateMovesForPlayer = function (player, output) {
    var pieces = player.piecesOnBoard.slice();
    for (var i = 0; i < pieces.length; i++) {

        var piece = pieces[i];
        var moveTemplate = new Move(piece.ownerPlayer, this, piece, piece.position);

        /*// get promotion possibilities
        piece.pieceType.promotionOpportunities.each(function (op) {
            if (!op.mandatory && op.type != PromotionType.EndOfMove && op.isAvailable(moveTemplate, this.game))
                op.options.each(function (option) {
                    output.push(new Promotion(piece.ownerPlayer, piece, option, this.moveNumber, op.type == PromotionType.CountsAsMove));
                });
        });
        */
        // and then get move possibilities
        for (var j = 0; j < piece.pieceType.moves.length; j++) {
            var moveDef = piece.pieceType.moves[j];
            var possibilities = moveDef.appendValidNextSteps(moveTemplate, piece, null);
            for (var k = 0; k < possibilities.length; k++) {
                var move = possibilities[k];

                if (this.game.endOfGame.illegalMovesSpecified) {
                    move.perform(this.game, false);
                    var result = this.game.endOfGame.checkEndOfTurn(this, move);
                    move.reverse(this.game, false);
                    if (result == EndOfGame.Type.IllegalMove)
                        continue;
                }

                output.push(move);
            }
        }
    }

    // now look at each held piece
    /*
    if (player.piecesHeld.length == 0)
        continue;
    
    pieces = player.piecesHeld.slice();
    this.game.board.cells.items.each(function (coord, cell) {
        if (cell.value != Board.CellType.Normal)
            return;
    
        for (var i = 0; i < pieces.length; i++) {
            var move = new Move(piece.ownerPlayer, this, piece, null);
            move.addStep(MoveStep.CreateDrop(piece, coord, piece.ownerPlayer));

            if (this.game.rules.dropPiecesWhen == null || this.game.rules.dropPiecesWhen.isSatisfied(move, this))
                output.push(move);
        }
    });
    */
}

// sort possible states (moves) by piece, determine notation for each move (done all at once to ensure they are unique), and return whether any moves are possible or not
GameState.prototype.prepareMovesForTurn = function () {
    var allMoves = {};
    var anyMoves = false;
    this.possibleMovesByPiece = {};

    var moves = this.determinePossibleMoves();

    for (var i = 0; i < moves.length; i++) {
        anyMoves = true;
        var move = moves[i];

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