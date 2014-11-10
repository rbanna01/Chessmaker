function Move(player, piece, startPos, moveNum) {
    this.player = player;
    this.piece = piece;
    this.startPos = startPos;
    this.moveNumber = moveNum;

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
    for (var i = 0; i < this.steps.length; i++)
        if (!this.steps[i].perform(game, updateDisplay)) // move failed, roll-back
        {
            for (var j = i - 1; j >= 0; j--)
                if (!this.steps[j].reverse(game, updateDisplay))
                    throw "Move failed on step " + i + "/" + this.steps.length + ", and rolling move back then failed on step " + j + ". Unable to rectify board state.";

            return false;
        }

    if (this.piece.moveNumber == 1)
        this.piece.firstMoveTurn = game.TurnNumber;

    this.piece.lastMoveTurn = game.TurnNumber;
    this.piece.moveNumber++;

    return true;
};

Move.prototype.reverse = function (game, updateDisplay) {
    for (var i = this.steps.length - 1; i >= 0; i--)
        if (!this.steps[i].reverse(game, updateDisplay)) // move failed, roll-back
        {
            for (var j = i + 1; j < steps.length; j++)
                if (!this.steps[j].perform(game, updateDisplay))
                    throw "Reversing move failed on step " + i + "/" + this.steps.length + " (counting backwards), and rolling move back then failed on step " + j + ". Unable to rectify board state.";

            return false;
        }

    this.piece.moveNumber--;
    if (this.piece.firstMoveTurn == game.TurnNumber)
        this.piece.firstMoveTurn = null;

    return true;
};

Move.prototype.addPieceReference = function(piece, ref) {
    this.references[ref] = piece;
};

Move.prototype.getPieceByRef = function (ref) {
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
    }
    return this.startPos;
};

Move.prototype.getAllPositions = function () {
    var allPos = [];
    for (var i = 0; i < this.steps.length; i++)
        if (this.steps[i].piece == this.piece && this.steps[i].toState == PieceState.OnBoard)
            allPoints.push(this.steps[i].toPos);
    return allPos;
};