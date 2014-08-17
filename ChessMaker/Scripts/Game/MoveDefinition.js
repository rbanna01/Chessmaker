function MoveDefinition() {
    
}

MoveDefinition.prototype.allowTopLevel = true;

MoveDefinition.parseDirections = function(xmlNode) {
    // load absolute directions, relative directions and direction groups.
};

MoveDefinition.parse = function(xmlNode) {
    switch ($(xmlNode).get(0).nodeName.toLowerCase()) {
        case "slide":
            return Slide.parse(xmlNode);
        case "leap":
            return Leap.parse(xmlNode);
        case "hop":
            return Hop.parse(xmlNode);
        case "shoot":
            return Shoot.parse(xmlNode);
        case "move_like":
            return MoveLike.parse(xmlNode);
        case "sequence":
            if (!isTopLevel)
                throw xmlNode.Name + " only allowed at top level!";
            return Sequence.parse(xmlNode);
        case "repeat":
            if (isTopLevel)
                throw xmlNode.Name + " not allowed at top level!";
            return Repeat.parse(xmlNode);
        case "when_possible":
            if (isTopLevel)
                throw xmlNode.Name + " not allowed at top level!";
            return WhenPossible.parse(xmlNode);
        case "reference_piece":
            if (isTopLevel)
                throw xmlNode.Name + " not allowed at top level!";
            return ReferencePiece.parse(xmlNode);
        case "arbitrary_attack":
            if (isTopLevel)
                throw xmlNode.Name + " not allowed at top level!";
            return ArbitraryAttack.parse(xmlNode);
        default:
            throw "Unexpected move type: " + $(xmlNode).get(0).nodeName.toLowerCase();
    }
}

MoveDefinition.When = {
    Any: 0,
    Move: 1,
    Capture: 2
};

MoveDefinition.Owner = {
    Any: 0,
    Self: 1,
    Enemy: 2,
    Ally: 3
};



function Slide() {

}

extend(Slide, MoveDefinition);

Slide.parse = function(xmlNode) {
    return new Slide();
}


function Leap() {

}

extend(Leap, MoveDefinition);

Leap.parse = function (xmlNode) {
    return new Leap();
}


function Hop() {

}

extend(Hop, MoveDefinition);

Hop.parse = function (xmlNode) {
    return new Hop();
}


function Shoot() {

}

extend(Shoot, MoveDefinition);

Shoot.parse = function (xmlNode) {
    return new Shoot();
}


function MoveLike() {

}

extend(MoveLike, MoveDefinition);

MoveLike.parse = function (xmlNode) {
    return new MoveLike();
}


function ReferencePiece() {

}

extend(ReferencePiece, MoveDefinition);
ReferencePiece.prototype.allowTopLevel = false;

ReferencePiece.parse = function (xmlNode) {
    return new ReferencePiece();
}


function ArbitraryAttack() {

}

extend(ArbitraryAttack, MoveDefinition);
ArbitraryAttack.prototype.allowTopLevel = false; // possible argument for allowing these at top level

ArbitraryAttack.parse = function (xmlNode) {
    return new ArbitraryAttack();
}


function MoveGroup() {

}

extend(MoveGroup, MoveDefinition);


function Sequence() {

}

extend(Sequence, MoveGroup);

Sequence.parse = function (xmlNode) {
    return new Sequence();
}


function Repeat() {

}

extend(Repeat, MoveGroup);

Repeat.parse = function (xmlNode) {
    return new Repeat();
}


function WhenPossible() {

}

extend(WhenPossible, MoveGroup);

WhenPossible.parse = function (xmlNode) {
    return new WhenPossible();
}