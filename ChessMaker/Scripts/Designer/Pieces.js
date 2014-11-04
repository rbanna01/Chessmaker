Slide.prototype.save = function (parent) {
    var node = parent.ownerDocument.createElement('slide');

    if (this.piece !== 'self')
        node.setAttribute('piece', this.piece);

    node.setAttribute('dir', this.dir);
    node.setAttribute('dist', this.dist);
    node.setAttribute('distLimit', this.distLimit);
    node.setAttribute('when', this.moveWhen);

    parent.appendChild(node);
    if (this.conditions !== null)
        this.conditions.save(node);
}

Leap.prototype.save = function (parent) {
    var node = parent.ownerDocument.createElement('leap');

    if (this.piece !== 'self')
        node.setAttribute('piece', this.piece);

    node.setAttribute('dir', this.dir);
    node.setAttribute('dist', this.dist);
    node.setAttribute('distLimit', this.distLimit);
    node.setAttribute('secondDist', this.secondDist);
    node.setAttribute('secondDir', this.secondDir);
    node.setAttribute('when', this.moveWhen);

    parent.appendChild(node);
    if (this.conditions !== null)
        this.conditions.save(node);
}

Hop.prototype.save = function (parent) {
    var node = parent.ownerDocument.createElement('hop');

    if (this.piece !== 'self')
        node.setAttribute('piece', this.piece);

    node.setAttribute('dir', this.dir);
    node.setAttribute('distToHurdle', this.distToHurdle);
    node.setAttribute('distAfterHurdle', this.distAfteroHurdle);
    node.setAttribute('when', this.moveWhen);
    node.setAttribute('captureHurdle', this.captureHurdle);

    parent.appendChild(node);
    if (this.conditions !== null)
        this.conditions.save(node);
}

Shoot.prototype.save = function (parent) {
    var node = parent.ownerDocument.createElement('shoot');

    if (this.piece !== 'self')
        node.setAttribute('piece', this.piece);

    node.setAttribute('dir', this.dir);
    node.setAttribute('dist', this.dist);
    node.setAttribute('distLimit', this.distLimit);
    node.setAttribute('secondDist', this.secondDist);
    node.setAttribute('secondDir', this.secondDir);
    node.setAttribute('when', this.moveWhen);

    parent.appendChild(node);
    if (this.conditions !== null)
        this.conditions.save(node);
}

MoveLike.prototype.save = function (parent) {
    var node = parent.ownerDocument.createElement('moveLike');

    node.setAttribute('other', this.piece);
    node.setAttribute('when', this.moveWhen);

    parent.appendChild(node);
    if (this.conditions !== null)
        this.conditions.save(node);
}

ReferencePiece.prototype.save = function (parent) {
    var node = parent.ownerDocument.createElement('referencePiece');

    node.setAttribute('name', this.refName);

    if (this.direction !== null)
        node.setAttribute('dir', this.direction);

    node.setAttribute('dist', this.distance);

    if (this.otherType !== 'any')
        node.setAttribute('type', this.otherType);

    node.setAttribute('owner', this.otherOwner);

    parent.appendChild(node);
};

ArbitraryAttack.prototype.save = function (parent) {
    var node = parent.ownerDocument.createElement('arbitraryAttack');

    node.setAttribute('row_ref', this.piece);
    node.setAttribute('col_ref', this.moveWhen);
    node.setAttribute('row_offset', this.piece);
    node.setAttribute('col_offset', this.moveWhen);
    node.setAttribute('move', this.moveWithAttack.toString());

    parent.appendChild(node);
    if (this.conditions !== null)
        this.conditions.save(node);
};

Sequence.prototype.save = function (parent) {
    var node = parent.ownerDocument.createElement('sequence');

    for (var i = 0; i < this.contents.length; i++)
        this.contents[i].save(node);

    parent.appendChild(node);
};

Repeat.prototype.save = function (parent) {
    var node = parent.ownerDocument.createElement('repeat');

    for (var i = 0; i < this.contents.length; i++)
        this.contents[i].save(node);

    parent.appendChild(node);
};

WhenPossible.prototype.save = function (parent) {
    var node = parent.ownerDocument.createElement('whenPossible');

    for (var i = 0; i < this.contents.length; i++)
        this.contents[i].save(node);

    parent.appendChild(node);
};

Conditions.prototype.save = function(parent) {

}

$(function () {

});