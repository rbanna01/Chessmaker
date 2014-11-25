"use strict";

function MoveStep(piece) {
    this.stepID = MoveStep.nextStepID++;
    this.piece = piece;
    this.fromState = null;
    this.toState = null;
    this.fromPos = null;
    this.toPos = null;
    this.fromOwner = piece.ownerPlayer;
    this.toOwner = piece.ownerPlayer;
    this.fromStateOwner = null;
    this.toStateOwner = null;
    this.fromType = piece.pieceType;
    this.toType = piece.pieceType;

    this.direction = null; // used only for resolving relative directions of subsequent steps
}

MoveStep.nextStepID = 1;

MoveStep.prototype.perform = function (game, updateDisplay, log) {
    if (!this.pickup(this.fromState, this.fromStateOwner, this.fromPos, this.fromOwner, this.fromType))
        return false;
    
    this.place(this.toState, this.toStateOwner, this.toPos, this.toOwner, this.toType);
    
    if (log)
        console.log('> moved ' + this.piece.pieceType.name + ' from ' + this.fromPos.name + ' to ' + this.toPos.name);

    if (updateDisplay)
        this.updateDisplay();

    return true;
};

MoveStep.prototype.reverse = function (game, updateDisplay, log) {
    if (!this.pickup(this.toState, this.toStateOwner, this.toPos, this.toOwner, this.toType))
        return false;

    this.place(this.fromState, this.fromStateOwner, this.fromPos, this.fromOwner, this.fromType);

    if (log)
        console.log('< moved ' + this.piece.pieceType.name + ' from ' + this.toPos.name + ' to ' + this.fromPos.name);

    if (updateDisplay)
        this.updateDisplay();

    return true;
};

MoveStep.prototype.pickup = function (state, stateOwner, pos, owner, type) {
    if (this.piece.pieceState != state) // piece isn't in the expected state, quit
    {
        console.log('state is wrong: got ' + this.piece.pieceState + ', expected ' + state + ' for ' + this.piece.ownerPlayer.name + ' ' + this.piece.pieceType.name);
        return false;
    }

    if (this.piece.ownerPlayer != owner) {
        console.log('owner is wrong: got ' + this.piece.ownerPlayer.name + ', expected ' + owner.name + ' for ' + this.piece.ownerPlayer.name + ' ' + this.piece.pieceType.name);
        return false;
    }

    if (this.piece.stateOwner != stateOwner) {
        console.log('state owner is wrong: got ' + (this.piece.stateOwner == null ? '[null]' : this.piece.stateOwner.name) + ', expected ' + (stateOwner == null ? '[null]' : stateOwner.name) + ' for ' + this.piece.ownerPlayer.name + ' ' + this.piece.pieceType.name);
        return false;
    }

    switch (state) {
        case Piece.State.OnBoard:
            if (this.piece.position != pos) {
                console.log('position is wrong: got ' + this.piece.position.name + ', expected ' + pos.name + ' for ' + this.piece.ownerPlayer.name + ' ' + this.piece.pieceType.name);
                return false;
            }
            if (this.toState != this.fromState || this.fromOwner != this.toOwner)
                arrayRemoveItem(owner.piecesOnBoard, this.piece);
            pos.piece = null;
            return true;
        case Piece.State.Captured:
            if (stateOwner == null || !arrayRemoveItem(stateOwner.piecesCaptured, this.piece))
                return false; // wasn't captured by that player after all ... can't perform this action
            return true;
        case Piece.State.Held:
            if (stateOwner == null || !arrayRemoveItem(stateOwner.piecesHeld, this.piece))
                return false; // wasn't held by that player after all ... can't perform this action
            return true;
        default:
            throw 'Unexpected piece state in MoveStep.pickup: ' + state;
    }
};

MoveStep.prototype.place = function (state, stateOwner, pos, owner, type) {
    this.piece.pieceState = state;
    this.piece.stateOwner = stateOwner;
    this.piece.position = pos;
    this.piece.ownerPlayer = owner;
    this.piece.type = type;
    
    switch (state) {
        case Piece.State.OnBoard:
            pos.piece = this.piece;
            if (this.toState != this.fromState || this.fromOwner != this.toOwner)
                owner.piecesOnBoard.push(this.piece);
            break;
        case Piece.State.Captured:
            stateOwner.piecesCaptured.push(this.piece);
            break;
        case Piece.State.Held:
            stateOwner.piecesHeld.push(this.piece);
        default:
            throw 'Unexpected piece state in MoveStep.place: ' + state;
    }
};

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

MoveStep.CreateMove = function (piece, from, to, dir) {
    var s = new MoveStep(piece);
    s.fromPos = from;
    s.toPos = to;
    s.fromState = Piece.State.OnBoard;
    s.toState = Piece.State.OnBoard;
    s.direction = dir;
    return s;
};

MoveStep.CreateCapture = function (piece, from, capturedBy, toHeld) {
    var s = new MoveStep(piece);
    s.fromPos = from;
    s.fromState = Piece.State.OnBoard;

    s.toState = toHeld ? Piece.State.Held : Piece.State.Captured;
    s.toStateOwner = capturedBy;
    return s;
};

MoveStep.CreateDrop = function (piece, to, droppedBy) {
    var s = new MoveStep(piece);
    s.fromState = Piece.State.Held;
    s.fromStateOwner = dropped;

    s.toPos = to;
    s.toState = Piece.State.OnBoard;
    return s;
};

MoveStep.CreatePromotion = function (piece, fromType, toType) {
    var s = new MoveStep(piece);
    s.fromState = s.toState = piece.state;
    s.fromStateOwner = s.toStateOwner = piece.stateOwner;
    s.fromPos = s.toPos = piece.position;

    s.fromType = fromType;
    s.toType = toType;
    return s;
};

MoveStep.CreateSteal = function (piece, fromOwner, toOwner) {
    var s = new MoveStep(piece);
    s.fromState = s.toState = piece.state;
    s.fromStateOwner = s.toStateOwner = piece.stateOwner;
    s.fromPos = s.toPos = piece.position;

    s.fromOwner = fromOwner;
    s.toOwner = toOwner;
    return s;
};