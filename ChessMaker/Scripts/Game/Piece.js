"use strict";

function Piece(owner, type, pos, state, stateOwner) {
}

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
