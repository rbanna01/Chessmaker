function Piece(owner, type, pos, state, stateOwner) {
    this.elementID = "p" + (Piece.nextID++);
    this.ownerPlayer = owner;
    this.position = pos;
    this.pieceType = type;
    this.pieceState = state;
    this.stateOwner = stateOwner;
    this.moveNumber = 1;      // how is this set, when loaded?
    this.lastMoveTurn = 0; // how is this set, when loaded?
}

Piece.nextID = 1;

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