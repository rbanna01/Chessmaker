function Piece(owner, type, pos, state, stateOwner) {
    this.elementID = "p" + (Piece.nextID++);
    this.ownerPlayer = owner;
    this.position = pos;
    this.pieceType = type;
    this.pieceState = state;
    this.stateOwner = stateOwner;
    this.moveNumber = 1;      // how is this set, when loaded?
    this.firstMoveTurn = null;// how is this set, when loaded?
    this.lastMoveTurn = null; // how is this set, when loaded?

    this.cachedMoves = null;
}

Piece.nextID = 1;

Piece.prototype.determinePossibleMoves = function (game) {
    var moves = [];

    if (this.pieceState == Piece.State.OnBoard) {
        var moveTemplate = new Move(this.ownerPlayer, this, this.position, game.moveNumber);

        /*// get promotion possibilities
        this.pieceType.promotionOpportunities.each(function (op) {
            if (!op.mandatory && op.type != PromotionType.EndOfMove && op.isAvailable(moveTemplate, game))
                op.options.each(function (option) {
                    piece.possibleMoves.push(new Promotion(piece.ownerPlayer, piece, option, game.moveNumber, op.type == PromotionType.CountsAsMove));
                });
        });
        */
        // and then get move possibilities
        for (var i = 0; i < this.pieceType.moves.length; i++) {
            var move = this.pieceType.moves[i];
            var possibilities = move.appendValidNextSteps(moveTemplate, this, game, null);
            for (var j = 0; j < possibilities.length; j++)
                moves.push(possibilities[j]);
        }
    }/*
    else if (this.pieceState == Piece.State.Held) {
        game.board.cells.items.each(function (coord, cell) {
            if (cell.value != Board.CellType.Normal)
                return;

            var move = new Move(piece.ownerPlayer, piece, null, game.moveNumber, true);
            move.addStep(MoveStep.CreateDrop(piece, coord, piece.ownerPlayer));

            if (game.rules.dropPiecesWhen == null || game.rules.dropPiecesWhen.isSatisfied(move, game))
                piece.possibleMoves.push(move);
        });
    }*/

    return moves;
};

Piece.prototype.canCapture = function (targetPiece) {
    return this.ownerPlayer.getRelationship(targetPiece.ownerPlayer) == Player.Relationship.Enemy;
};

Piece.prototype.typeMatches = function (strType) {
    if (strType == "any")
        return true;/*
    if (strType == "royal")
        return this.pieceType.royalty == PieceType.RoyalState.Royal;
    if (strType == "antiroyal")
        return this.pieceType.royalty == PieceType.RoyalState.AntiRoyal;*/
    return this.pieceType.name == strType;
};

Piece.prototype.isThreatenedAt = function (game, testPos) {
    var restorePos = this.position;
    this.position = testPos;

    /*
    todo: implement the rest of this function, once game turn numbers, game piece lists and possible moves are implemented
    */

    this.position = restorePosition;
    //game.turnNumber = game.actualTurnNumber;
    return false;
};

Piece.prototype.createImage = function () {
    var image = SVG('use');
    image.setAttribute('id', this.elementID);
    image.setAttribute('class', 'piece ' + this.ownerPlayer.name);
    image.setAttribute('x', this.position.coordX);
    image.setAttribute('y', this.position.coordY);
    this.updateImage(image);
    return image;
};

Piece.prototype.getImage = function () {
    return document.getElementById(this.elementID);
}

Piece.prototype.updateImage = function (image) {
    image.setAttributeNS('http://www.w3.org/1999/xlink', 'href', this.pieceType.appearances[this.ownerPlayer.name]);
}

Piece.State = {
    OnBoard: 1,
    Captured: 2,
    Held: 3
}