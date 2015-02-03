"use strict";

function Piece(owner, type, pos, state, stateOwner) {
}

Piece.prototype.getImage = function () {
    return document.getElementById(this.elementID);
}

Piece.prototype.updateImage = function (image) {
    image.setAttributeNS('http://www.w3.org/1999/xlink', 'href', this.pieceType.appearances[this.ownerPlayer.name]);
}
