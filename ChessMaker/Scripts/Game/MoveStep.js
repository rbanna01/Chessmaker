function MoveStep(piece) {
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
}

MoveStep.prototype.perform = function (board, updateDisplay) {
    if (!this.pickup(this.fromState, this.fromStateOwner, this.fromPos, this.fromOwner, this.fromType))
        return false;
    
    this.place(this.toState, this.toStateOwner, this.toPos, this.toOwner, this.toType);

    if (updateDisplay)
        this.updateDisplay();
};

MoveStep.prototype.reverse = function (board, updateDisplay) {
    if (!this.pickup(this.toState, this.toStateOwner, this.toPos, this.toOwner, this.toType))
        return false;

    this.place(this.fromState, this.fromStateOwner, this.fromPos, this.fromOwner, this.fromType);

    if (updateDisplay)
        this.updateDisplay();
};

MoveStep.prototype.pickup = function (state, stateOwner, pos, owner, type) {
    if (this.piece.pieceState != state) // piece isn't in the expected state, quit
    {
        console.log('state is wrong: got ' + this.piece.pieceState + ', expected ' + state + ' for ' + this.piece.ownerPlayer.name + ' ' + this.piece.pieceType.name);
        return false;
    }

    if (this.piece.owner != owner) {
        console.log('state owner is wrong: got ' + this.piece.owner + ', expected ' + owner + ' for ' + this.piece.ownerPlayer.name + ' ' + this.piece.pieceType.name);
        return false;
    }

    if (this.piece.stateOwner != stateOwner) {
        console.log('state owner is wrong: got ' + (this.piece.stateOwner == null ? '[null]' : this.piece.stateOwner) + ', expected ' + (stateOwner == null ? '[null]' : stateOwner) + ' for ' + this.piece.ownerPlayer.name + ' ' + this.piece.pieceType.name);
        return false;
    }

    switch (state) {
        case Piece.State.OnBoard:
            if (this.piece.position != pos) {
                console.log('position is wrong: got ' + this.piece.position.name + ', expected ' + pos.name + ' for ' + this.piece.ownerPlayer.name + ' ' + this.piece.pieceType.name);
                return false;
            }
            this.piece.ownerPlayer.piecesOnBoard.removeItem(this.piece);
            cell.piece = null;
            return true;
        case Piece.State.Captured:
            if (stateOwner == null || !stateOwner.piecesCaptured.removeItem(this.piece))
                return false; // wasn't captured by that player after all ... can't perform this action
            return true;
        case Piece.State.Held:
            if (stateOwner == null || !stateOwner.piecesHeld.removeItem(this.piece))
                return false; // wasn't held by that player after all ... can't perform this action
            return true;
        default:
            throw 'Unexpected piece state in MoveStep.pickup: ' + state;
    }
};

MoveStep.prototype.place = function (state, stateOwner, pos, owner, type) {
    if (this.piece.ownerPlayer != owner) {
        this.piece.ownerPlayer = owner;
        this.piece.updateImage();
    }
    
    if (this.piece.type != type) {
        this.piece.type = type;
        this.piece.updateImage();
    }

    switch (state) {
        case Piece.State.OnBoard:
            this.piece.position = pos;
            this.piece.state = state;
            this.piece.stateOwner = null;
            pos.piece = this.piece;
            owner.piecesOnBoard.push(this.piece);
            break;
        case Piece.State.Captured:
            this.piece.position = null;
            this.piece.state = state;
            stateOwner.piecesCaptured.push(this.piece);
            break;
        case Piece.State.Held:
            this.piece.position = null;
            this.piece.state = state;
            stateOwner.piecesHeld.push(this.piece);
        default:
            throw 'Unexpected piece state in MoveStep.place: ' + state;
    }
};

MoveStep.prototype.updateDisplay = function () {
    var image = this.piece.getImage();

    if (this.fromPos != this.toPos) {
        /*
        doesn't currently animate
        */
        image.setAttribute('x', this.piece.position.coordX);
        image.setAttribute('y', this.piece.position.coordY);
    }
    else if (this.fromState != this.toState || this.fromStateOwner != this.toStateOwner) {
        /*
        don't have anywhere to put this at present.
        move piece into the correct hand of the correct player.
        */
    }
};

MoveStep.CreateMove = function (piece, from, to) {
    var s = new MoveStep(piece);
    s.fromPos = from;
    s.toPos = to;
    s.fromState = Piece.State.OnBoard;
    s.toState = Piece.State.OnBoard;
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