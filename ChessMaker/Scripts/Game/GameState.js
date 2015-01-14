"use strict";

function GameState(game, prevMove, moveNum) {
    this.game = game;
    this.move = prevMove;
    this.currentPlayer = null;
    this.moveNumber = moveNum;

    this.possibleMoves = null;
    this.possibleMovesByPiece = null;
    this.threatCheckMoves = {}; // these are never actually performed or followed up on, but are used in calculating whether moves would be disallowed due to check. e.g. the king's forward "attack" in xiangqui.
}

GameState.prototype.determinePossibleMoves = function (player) {
    if (this.possibleMoves === null) {
        this.possibleMoves = [];

        MoveStep.nextStepID = 1;
        this.calculateMovesForPlayer(player, this.possibleMoves);
    }
    return this.possibleMoves;
};

GameState.prototype.determineThreatMoves = function (player, prevStep) {
    var stepID = prevStep == null ? '' : prevStep.stepID;

    if (!this.threatCheckMoves.hasOwnProperty(stepID)) {

        for (var p = 0; p < game.players.length; p++) {
            var opponent = game.players[p];

            if (player.getRelationship(opponent) != Player.Relationship.Enemy)
                continue;

            var threatMoves = [];
            this.calculateMovesForPlayer(opponent, threatMoves);
            this.threatCheckMoves[stepID] = threatMoves;
        }
    }

    return this.threatCheckMoves[stepID];
};

GameState.prototype.calculateMovesForPlayer = function (player, output) {
    var pieces = player.piecesOnBoard.slice();
    for (var i = 0; i < pieces.length; i++) {

        var piece = pieces[i];
        var moveTemplate = new Move(piece.ownerPlayer, piece, piece.position, this.moveNumber);

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
            var move = piece.pieceType.moves[j];
            var possibilities = move.appendValidNextSteps(moveTemplate, piece, this, null);
            for (var k = 0; k < possibilities.length; k++)
                output.push(possibilities[k]);
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
            var move = new Move(piece.ownerPlayer, piece, null, this.moveNumber, true);
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

    if (this.possibleMoves === null)
        this.determinePossibleMoves(this.currentPlayer);

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