function MoveDefinition(pieceRef, dir, when, conditions) {
    this.piece = pieceRef;
    this.dir = dir;
    this.moveWhen = when;
    this.conditions = Conditions.parse(conditions);
}

MoveDefinition.parse = function (xmlNode, isTopLevel) {
    switch (xmlNode.nodeName) {
        case "slide":
            return Slide.parse(xmlNode);
        case "leap":
            return Leap.parse(xmlNode);
        case "hop":
            return Hop.parse(xmlNode);
        case "shoot":
            return Shoot.parse(xmlNode);
        case "moveLike":
            return MoveLike.parse(xmlNode);
        case "sequence":
            if (!isTopLevel)
                throw xmlNode.Name + " only allowed at top level!";
            return Sequence.parse(xmlNode);
        case "repeat":
            if (isTopLevel)
                throw xmlNode.Name + " not allowed at top level!";
            return Repeat.parse(xmlNode);
        case "whenPossible":
            if (isTopLevel)
                throw xmlNode.Name + " not allowed at top level!";
            return WhenPossible.parse(xmlNode);
        case "referencePiece":
            if (isTopLevel)
                throw xmlNode.Name + " not allowed at top level!";
            return ReferencePiece.parse(xmlNode);
        case "arbitraryAttack":
            if (isTopLevel)
                throw xmlNode.Name + " not allowed at top level!";
            return ArbitraryAttack.parse(xmlNode);
        default:
            throw "Unexpected move type: " + xmlNode.nodeName;
    }
};

MoveDefinition.When = {
    Any: 0,
    Move: 1,
    Capture: 2
};

MoveDefinition.When.parse = function (val) {
    if (val === undefined)
        return MoveDefinition.When.Any;

    if (val == "capture")
        return MoveDefinition.When.Capture;
    else if(val == "move")
        return MoveDefinition.When.Move;
    else if (val == "any")
        return MoveDefinition.When.Any;
    else
        throw "Unexpected when value: " + val;
};

MoveDefinition.Owner = {
    Any: 0,
    Self: 1,
    Enemy: 2,
    Ally: 3
};

MoveDefinition.Owner.parse = function (val) {
    if (val === undefined)
        return "any";
    switch (val) {
        case "any":
            return MoveDefinition.Owner.Any;
        case "self":
            return MoveDefinition.Owner.Self;
        case "enemy":
            return MoveDefinition.Owner.Enemy;
        case "ally":
            return MoveDefinition.Owner.Ally;
        default:
            throw "Unexpected owner value: " + val;
    }
};


function Slide(pieceRef, dir, dist, distLimit, when, conditions) {
    MoveDefinition.call(this, pieceRef, dir, when, conditions);
    this.dist = dist;
    this.distLimit = distLimit;
}

extend(Slide, MoveDefinition);

Slide.prototype.appendValidNextSteps = function (move, piece, game, previousStep) {
    var moves = [];
    
    // this is just a placeholder, move to any adjacent square!
    for (var dir in piece.position.links) {
        var adjacent = piece.position.links[dir];

        var captureStep;
        if (adjacent.piece != null) {
            var captured = adjacent.piece;
            
            if (captured.ownerPlayer == piece.ownerPlayer)
                continue; // don't capture your own
            
            captureStep = MoveStep.CreateCapture(captured, captured.position, piece.ownerPlayer, false);
        }

        var move = new Move(piece.ownerPlayer, piece, piece.position, game.moveNumber);

        if (captureStep !== undefined) {
            move.addStep(captureStep);
            captureStep = undefined;
        }

        move.addStep(MoveStep.CreateMove(piece, piece.position, adjacent));

        moves.push(move);
    }

    return moves;
};

Slide.parse = function (xmlNode) {
    var node = $(xmlNode);
    var conditions = node.children("conditions").children();

    var pieceRef = node.attr("piece");
    if (pieceRef === undefined)
        pieceRef = "self";

    var dir = node.attr("dir");

    var dist = Distance.parse(node.attr("dist"));
    var distLimit = Distance.parse(node.attr("distLimit"));
	var when = MoveDefinition.When.parse(node.attr("when"));

	return new Slide(pieceRef, dir, dist, distLimit, when, conditions);
}


function Leap(pieceRef, dir, dist, distLimit, secondDir, secondDist, when, conditions) {
    MoveDefinition.call(this, pieceRef, dir, when, conditions);
    this.dist = dist;
    this.distLimit = distLimit;
    this.secondDir = secondDir;
    this.secondDist = secondDist;
}

extend(Leap, MoveDefinition);

Leap.prototype.appendValidNextSteps = function (move, piece, game, previousStep) {
    var moves = [];

    return moves;
};

Leap.parse = function (xmlNode) {
    var node = $(xmlNode);
    var conditions = node.children("conditions").children();

    var pieceRef = node.attr("piece");
    if (pieceRef == undefined)
        pieceRef = "self";

    var dir = node.attr("dir");

    var dist = Distance.parse(node.attr("dist"));
    var distLimit = Distance.parse(node.attr("distLimit"));

    var secondDist = Distance.parse(node.attr("secondDist"));
    if (secondDist == null)
        secondDist = Distance.Zero;

    var secondDir = node.attr("secondDir");

    var when = MoveDefinition.When.parse(node.attr("when"));

    return new Leap(pieceRef, dir, dist, distLimit, secondDir, secondDist, when, conditions);
}


function Hop(pieceRef, dir, distToHurdle, distAfterHurdle, when, captureHurdle, conditions) {
    MoveDefinition.call(this, pieceRef, dir, when, conditions);
    this.distToHurdle = distToHurdle;
    this.distAfterHurdle = distAfterHurdle;
    this.captureHurdle = captureHurdle;
}

extend(Hop, MoveDefinition);

Hop.prototype.appendValidNextSteps = function (move, piece, game, previousStep) {
    var moves = [];

    return moves;
};

Hop.parse = function (xmlNode) {
    var node = $(xmlNode);
    var conditions = node.children("conditions").children();

    var pieceRef = node.attr("piece");
    if (pieceRef == undefined)
        pieceRef = "self";

    var dir = node.attr("dir");

    var distToHurdle = Distance.parse(node.attr("distToHurdle"));
    var distAfterHurdle = Distance.parse(node.attr("distAfterHurdle"));

    var when = MoveDefinition.When.parse(node.attr("when"));
    var captureHurdle = node.attr("captureHurdle") == "true";

    return new Hop(pieceRef, dir, distToHurdle, distAfterHurdle, when, captureHurdle, conditions);
}


function Shoot(pieceRef, dir, dist, distLimit, secondDir, secondDist, when, conditions) {
    MoveDefinition.call(this, pieceRef, dir, when, conditions);
    this.dist = dist;
    this.distLimit = distLimit;
    this.secondDir = secondDir;
    this.secondDist = secondDist;
}

extend(Shoot, MoveDefinition);

Shoot.prototype.appendValidNextSteps = function (move, piece, game, previousStep) {
    var moves = [];

    return moves;
};

Shoot.parse = function (xmlNode) {
    var node = $(xmlNode);
    var conditions = node.children("conditions").children();

    var pieceRef = node.attr("piece");
    if (pieceRef == undefined)
        pieceRef = "self";

    var dir = node.attr("dir");

    var dist = Distance.parse(node.attr("dist"));
    var distLimit = Distance.parse(node.attr("distLimit"));

    var secondDist = Distance.parse(node.attr("secondDist"));
    if (secondDist == null)
        secondDist = Distance.Zero;

    var secondDir = node.attr("secondDir");

    var when = null;/*XmlHelper.readWhen(xmlNode, "when");*/

    return new Shoot(pieceRef, dir, dist, distLimit, secondDir, secondDist, when, conditions);
}


function MoveLike(other, when, conditions) {
    MoveDefinition.call(this, pieceRef, undefined, when, conditions);
}

extend(MoveLike, MoveDefinition);

MoveLike.prototype.appendValidNextSteps = function (move, piece, game, previousStep) {
    var moves = [];

    return moves;
};

MoveLike.parse = function (xmlNode) {
    var node = $(xmlNode);
    var conditions = node.children("conditions").children();
    var pieceRef = xmlNode.attr("other");
    var when = MoveDefinition.When.parse(node.attr("when"));

    return new MoveLike(pieceRef, when, conditions);
}


function ReferencePiece(name, type, owner, dir, dist) {
    MoveDefinition.call(this, undefined, undefined, undefined, undefined);
    this.refName = name;
    this.otherType = type;
    this.otherOwner = owner;
    this.direction = dir;
    this.distance = dist;
}

extend(ReferencePiece, MoveDefinition);

ReferencePiece.prototype.appendValidNextSteps = function (move, piece, game, previousStep) {
    var moves = [];

    return moves;
};

ReferencePiece.parse = function (xmlNode) {
    var node = $(xmlNode);
    var name = node.attr("name");

    var dir = node.attr("dir");
    if (dir === undefined)
        dir = null;
    else if (DirectionGroups.hasOwnProperty(dir))
        throw "ReferencePiece requires a discreet direction, not a compound one!";

    var dist = Distance.parse(node.attr("dist"));

    var type = node.attr("type");
    if (type == undefined)
        type = "any";

    var owner = MoveDefinition.Owner.parse(node.attr("owner"));

    return new ReferencePiece(name, type, owner, dir, dist);
}


function ArbitraryAttack(rowRef, colRef, rowOffset, colOffset, moveWithAttack, conditions) {
    MoveDefinition.call(this, undefined, undefined, undefined, conditions);
    this.rowRef = rowRef;
    this.colRef = colRef;
    this.rowOffset = rowOffset;
    this.colOffset = colOffset;
    this.moveWithAttack = moveWithAttack;
}

extend(ArbitraryAttack, MoveDefinition);

ArbitraryAttack.prototype.appendValidNextSteps = function (move, piece, game, previousStep) {
    var moves = [];

    return moves;
};

ArbitraryAttack.parse = function (xmlNode) {
    var node = $(xmlNode);
    var conditions = node.children("conditions").children();
    var rowRef = node.attr("rowRef");
    var colRef = node.attr("colRef");
    var rowOffset = parseInt(node.attr("rowOffset"));
    var colOffset = parseInt(node.attr("colOffset"));
    var move = node.attr("move") == "true";

    return new ArbitraryAttack(rowRef, colRef, rowOffset, colOffset, conditions);
}


function MoveGroup(minOccurs, maxOccurs, stepOutIfFail) {
    MoveDefinition.call(this, undefined, undefined, undefined, undefined);
    this.contents = [];
    this.minOccurs = minOccurs;
    this.maxOccurs = maxOccurs;
    this.StepOutIfFail = stepOutIfFail; // true for whenPossible, false otherwise - makes a group obligatory unless it fails.
}

extend(MoveGroup, MoveDefinition);

MoveGroup.prototype.appendValidNextSteps = function (move, piece, game, previousStep) {
    var moves = [];

    return moves;
};


function Sequence() {
    MoveGroup.call(this, 1, 1, false);
}

extend(Sequence, MoveGroup);

Sequence.parse = function (xmlNode) {
    var node = $(xmlNode);
    var s = new Sequence();
    node.children().each(function (index, child) {
        s.contents.push(MoveDefinition.parse(child, false));
    });
    return s;
}


function Repeat(minOccurs, maxOccurs) {
    MoveGroup.call(this, minOccurs, maxOccurs, false);
}

extend(Repeat, MoveGroup);

Repeat.parse = function (xmlNode) {
    var node = $(xmlNode);
    var min = parseInt(node.attr("min"));
	var max = node.attr("max");
	if(max == "unbounded")
		max = -1;
	else
		max = parseInt(max);

    var r = new Repeat();
    node.children().each(function (index, child) {
        r.contents.push(MoveDefinition.parse(child, false));
    });
    return r;
}


function WhenPossible() {
    MoveGroup.call(this, 1, 1, true);
}

extend(WhenPossible, MoveGroup);

WhenPossible.parse = function (xmlNode) {
    var node = $(xmlNode);
    var w = new WhenPossible();
    node.children().each(function (index, child) {
        w.contents.push(MoveDefinition.parse(child, false));
    });
    return w;
}