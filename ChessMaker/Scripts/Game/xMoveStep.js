"use strict";

function MoveStep(piece) {
    
}

MoveStep.prototype.updateDisplay = function () {
    var image = this.piece.getImage();
    if (this.fromOwner != this.toOwner || this.fromType != this.toType)
        this.piece.updateImage(image);

    if (this.fromPos != this.toPos && this.toPos != null) {
        /*
        doesn't currently animate
        */
        image.setAttribute('x', this.piece.position.coordX);
        image.setAttribute('y', this.piece.position.coordY);
    }

    if (this.fromState != this.toState) {
        var oldParent = image.parentNode;
        oldParent.removeChild(image);
        var newParent;
        switch(this.piece.pieceState) {
            case Piece.State.OnBoard:
                newParent = document.getElementById('render'); break;
            case Piece.State.Captured:
                newParent = document.getElementById('captured'); break;
            case Piece.State.Held:
                newParent = document.getElementById('held'); break;
            default:
                throw 'Unexpected piece state in MoveStep.updateDisplay: ' + this.piece.pieceState;
        }

        if (newParent != null)
            newParent.appendChild(image);

        if (this.piece.pieceState != Piece.State.OnBoard)
            image.setAttribute('x', this.piece.ownerPlayer.pieceListX);

        if (oldParent.getAttribute('id') != 'render')
            game.board.updatePiecePositions(oldParent);
        if (newParent.getAttribute('id') != 'render')
            game.board.updatePiecePositions(newParent);
    }
    else if (this.fromStateOwner != this.toStateOwner) {
        image.setAttribute('x', this.piece.ownerPlayer.pieceListX);
        game.board.updatePiecePositions(image.parentNode);
    }
};