function Move(player, piece, startPos, moveNum) {
    this.player = player;
    this.piece = piece;
    this.startPos = startPos;
    this.moveNumber = moveNum;
    this.notation = '';
    this.prevPieceMoveTurn = null;

    this.steps = [];
    this.references = {};
}

Move.prototype.addStep = function(step) {
    this.steps.push(step);
};

Move.prototype.clone = function() {
    var move = new Move(this.player, this.piece, this.startPos, this.moveNumber);
		
    for ( var i=0; i<this.steps.length; i++ )
        move.addStep(this.steps[i]);

    for (var ref in this.references) {
        move.addPieceReference(ref, this.references[ref]);
    }

    return move;
};

Move.prototype.perform = function (game, updateDisplay) {
    for (var i = 0; i < this.steps.length; i++) {
        if (!this.steps[i].perform(game, updateDisplay)) // move failed, roll-back
        {
            for (var j = i - 1; j >= 0; j--)
                if (!this.steps[j].reverse(game, updateDisplay))
                    throw "Move failed on step " + i + "/" + this.steps.length + ", and rolling move back then failed on step " + j + ". Unable to rectify board state.";

            return false;
        }
    }

    this.prevPieceMoveTurn = this.piece.lastMoveTurn;
    this.piece.lastMoveTurn = game.moveNumber;
    this.piece.moveNumber++;
    game.moveNumber++;
    return true;
};

Move.prototype.reverse = function (game, updateDisplay) {
    for (var i = this.steps.length - 1; i >= 0; i--)
        if (!this.steps[i].reverse(game, updateDisplay)) // move failed, roll-back
        {
            for (var j = i + 1; j < this.steps.length; j++)
                if (!this.steps[j].perform(game, updateDisplay))
                    throw "Reversing move failed on step " + i + "/" + this.steps.length + " (counting backwards), and rolling move back then failed on step " + j + ". Unable to rectify board state.";

            return false;
        }

    this.piece.lastMoveTurn = this.prevPieceMoveTurn;
    this.piece.moveNumber--;
    game.moveNumber--;
    return true;
};

Move.prototype.addPieceReference = function(piece, ref) {
    this.references[ref] = piece;
};

Move.prototype.getPieceByRef = function (ref) {
    if (ref == 'self')
        return this.piece;
    var piece = this.references[ref];
    if (piece === undefined)
        return null;
    return piece;
};

Move.prototype.getEndPos = function () {
    for (var i = this.steps.length - 1; i >= 0; i--) {
        var s = this.steps[i];
        if (s.piece == this.piece && s.toState == Piece.State.OnBoard)
            return s.toPos;
    }// if this piece doesn't move, consider other pieces that it might move or capture
    for (var i = this.steps.length - 1; i >= 0; i--) {
        var s = this.steps[i];
        if (s.toState == Piece.State.OnBoard)
            return s.toPos;
        else if (s.fromState == Piece.State.OnBoard)
            return s.fromPos;
    }
    return this.startPos;
};

Move.prototype.getAllPositions = function () {
    var allPos = [];
    for (var i = 0; i < this.steps.length; i++)
        if (this.steps[i].piece == this.piece && this.steps[i].toState == Piece.State.OnBoard)
            allPoints.push(this.steps[i].toPos);
    return allPos;
};

Move.prototype.isCapture = function () {
    for (var i = 0; i < this.steps.length; i++) {
        var s = this.steps[i];
        if (s.piece != this.piece && s.toState != Piece.State.OnBoard)
            return true;
    }
    return false;
};

Move.prototype.getPromotionType = function () {
    for (var i = this.steps.length - 1; i >= 0; i--) {
        var s = this.steps[i];
        if (s.piece == this.piece && s.toType != s.fromType)
            return s.fromType;
    }
    return null;
};

Move.maxNotationDetail = 3;
Move.prototype.determineNotation = function (detailLevel) {
    // include an X if capturing a piece
    var capture = this.isCapture() ? 'x' : '';

    var desc;
    switch (detailLevel) {
        case 1:
            desc = this.piece.pieceType.notation + capture + this.getEndPos().name;
            break;
        case 2:
            desc = this.piece.pieceType.notation + this.startPos.name + capture + this.getEndPos().name;
            break;
        default:
            desc = this.piece.pieceType.notation + '(' + this.piece.elementID + ')' + this.startPos.name + capture + this.getEndPos().name;
            break;
    }

    // add promotion letter onto the end
    var promotion = this.getPromotionType();
    if (promotion != null)
        desc += promotion.notation;

    // add a plus to the end if its check. add a hash if its checkmate.

    // how to account for castling? some special moves simply need to specify their own special notation, i guess...

    var displayNumber = Math.ceil(this.moveNumber / game.players.length);
    if (this.moveNumber % game.players.length == 1) // first player's move
        displayNumber += '.  ';
    else
        displayNumber += '...';

    this.notation = displayNumber + desc;
    return this.notation;
};