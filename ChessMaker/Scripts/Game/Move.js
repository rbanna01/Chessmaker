function Move() {
    this.steps = [];
}

Move.prototype.addStep = function(step) {
    this.steps.push(step);
};

Move.prototype.clone = function() {
    var move = new Move();
		
    for ( var i=0; i<this.steps.length; i++ )
        move.addStep(this.steps[i]);
	/*
    this.references.each(function(ref, piece) {
        move.addPieceReference(piece, ref);
    });
	*/
    return move;
};

Move.prototype.perform = function (board, updateDisplay) {
    for (var i = 0; i < this.steps.length; i++)
        if (!this.steps[i].perform(board, updateDisplay)) // move failed, roll-back
        {
            for (var j = i - 1; j >= 0; j--)
                if (!this.steps[j].reverse(board, updateDisplay))
                    throw "Move failed on step " + i + "/" + this.steps.length + ", and rolling move back then failed on step " + j + ". Unable to rectify board state.";

            return false;
        }

    if (this.piece.moveNumber == 1)
        this.piece.firstMoveTurn = game.TurnNumber;

    this.piece.lastMoveTurn = game.TurnNumber;
    this.piece.moveNumber++;

    return true;
};

Move.prototype.reverse = function (board, updateDisplay) {
    for (var i = this.steps.length - 1; i >= 0; i--)
        if (!this.steps[i].reverse(board, updateDisplay)) // move failed, roll-back
        {
            for (var j = i + 1; j < steps.length; j++)
                if (!this.steps[j].perform(board, updateDisplay))
                    throw "Reversing move failed on step " + i + "/" + this.steps.length + " (counting backwards), and rolling move back then failed on step " + j + ". Unable to rectify board state.";

            return false;
        }

    this.piece.moveNumber--;
    if (this.piece.firstMoveTurn == game.TurnNumber)
        this.piece.firstMoveTurn = null;

    return true;
};
/*
Move.prototype.addPieceReference = function(piece, ref) {
    this.references.setItem(ref, piece);
};
	
Move.prototype.getPieceByRef = function (ref) {
    var piece = this.references.getItem(ref);
    if (piece == undefined)
        return null;
    return piece;
};
*/