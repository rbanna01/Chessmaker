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

Slide.prototype.appendValidNextSteps = function (baseMove, piece, game, previousStep) {
    var moves = [];

    if (this.piece != 'self')
    {// some steps will specify a different piece to act upon, rather than the piece being moved
        piece = baseMove.getPieceByRef(this.piece);
        if (piece == null) {
            console.log('piece ref not found: ' + this.piece);
            return null;
        }
    }
    
    var dirs = game.board.resolveDirection(this.dir, previousStep != null && previousStep.direction != null ? previousStep.direction : baseMove.player.forwardDir);
    
    for (var i = 0; i < dirs.length; i++) {
        var dir = dirs[i];

        var boardMaxDist = game.board.getMaxDistance(piece.position, dir);
        var distances = this.dist.getRange(this.distLimit, previousStep, boardMaxDist);
        var minDist = distances[0]; var maxDist = distances[1];
        var cell = piece.position;

        for (var dist = 1; dist <= maxDist; dist++) {
            cell = cell.links[dir];
            if (cell === undefined)
                break;

            var target = cell.piece;
            if (dist >= minDist) {
                var captureStep = null;
                if (this.moveWhen == MoveDefinition.When.Capture) {
                    if (target == null)
                        continue; // needs to be a capture for this slide to be valid, and there is no piece here. But there might be pieces beyond this one.
                    else if (piece.canCapture(target))
                        captureStep = MoveStep.CreateCapture(target, cell, piece.ownerPlayer, game.holdCapturedPieces);
                    else
                        break; // cannot capture this piece. Slides cannot pass over pieces, so there can be no more valid slides in this direction.
                }
                else if (this.moveWhen == MoveDefinition.When.Move && target != null)
                    break;
                else if (target != null) {
                    if (piece.canCapture(target))
                        captureStep = MoveStep.CreateCapture(target, cell, piece.ownerPlayer, game.holdCapturedPieces);
                    else
                        break; // cannot capture this piece (probably own it)
                }

                var move = baseMove.clone();
                if (captureStep != null) {
                    move.addStep(captureStep);
                    move.addPieceReference(target, 'target');
                }

                move.addStep(MoveStep.CreateMove(piece, piece.position, cell, dir));

                if (this.conditions.isSatisfied(move, game))
                    moves.push(move);
            }

            if (target != null)
                break; // Slides can't pass intervening pieces. As this cell was occupied, can be no more valid slides in this direction.
        }
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

Leap.prototype.appendValidNextSteps = function (baseMove, piece, game, previousStep) {
    var moves = [];

    if (this.piece != 'self') {// some steps will specify a different piece to act upon, rather than the piece being moved
        piece = baseMove.getPieceByRef(this.piece);
        if (piece == null) {
            console.log('piece ref not found: ' + this.piece);
            return null;
        }
    }
    
    var dirs = game.board.resolveDirection(this.dir, previousStep != null && previousStep.direction != null ? previousStep.direction : baseMove.player.forwardDir);

    for (var i = 0; i < dirs.length; i++) {
        var firstDir = dirs[i];

        var boardMaxDist = game.board.getMaxDistance(piece.position, firstDir);
        var distances = this.dist.getRange(this.distLimit, previousStep, boardMaxDist);
        var minDist = distances[0]; var maxDist = distances[1];

        var secondDirs = game.board.resolveDirection(this.secondDir, firstDir);
        for (var j = 0; j < secondDirs.length; j++) {
            var secondDir = secondDirs[j];

            boardMaxDist = game.board.getMaxDistance(piece.position, secondDir);
            distances = this.secondDist.getRange(null, previousStep, boardMaxDist);
            var minDist2 = distances[0]; var maxDist2 = distances[1];

            var straightCell = piece.position;

            for (var dist = 1; dist <= maxDist; dist++) {
                straightCell = straightCell.links[firstDir];
                if (straightCell === undefined)
                    break;

                if (dist < minDist)
                    continue; // not yet reached minimum straight distance, so don't turn

                var destCell = straightCell;
                for (var secondDist = 1; secondDist <= maxDist2; secondDist++) {
                    destCell = destCell.links[secondDir];
                    if (destCell === undefined)
                        break;

                    var target = destCell.piece;
                    if (secondDist >= minDist2) {
                        var captureStep = null;
                        if (this.moveWhen == MoveDefinition.When.Capture) {
                            if (target == null)
                                continue; // needs to be a capture for this slide to be valid, and there is no piece here. But there might be pieces beyond this one.
                            else if (piece.canCapture(target))
                                captureStep = MoveStep.CreateCapture(target, destCell, piece.ownerPlayer, game.holdCapturedPieces);
                            else
                                continue; // cannot capture this piece
                        }
                        else if (this.moveWhen == MoveDefinition.When.Move && target != null)
                            break;
                        else if (target != null) {
                            if (piece.canCapture(target))
                                captureStep = MoveStep.CreateCapture(target, destCell, piece.ownerPlayer, game.holdCapturedPieces);
                            else
                                continue; // cannot capture this piece
                        }

                        var move = baseMove.clone();
                        if (captureStep != null) {
                            move.addStep(captureStep);
                            move.addPieceReference(target, 'target');
                        }

                        move.addStep(MoveStep.CreateMove(piece, piece.position, destCell, secondDir));

                        if (this.conditions.isSatisfied(move, game))
                            moves.push(move);
                    }
                }
            }
        }


    }

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

Hop.prototype.appendValidNextSteps = function (baseMove, piece, game, previousStep) {
    var moves = [];

    if (this.piece != 'self') {// some steps will specify a different piece to act upon, rather than the piece being moved
        piece = baseMove.getPieceByRef(this.piece);
        if (piece == null) {
            console.log('piece ref not found: ' + this.piece);
            return null;
        }
    }

    var dirs = game.board.resolveDirection(this.dir, previousStep != null && previousStep.direction != null ? previousStep.direction : baseMove.player.forwardDir);

    
    for (var i = 0; i < dirs.length; i++) {
        var dir = dirs[i];

        var boardMaxDist = game.board.getMaxDistance(piece.position, dir);
        var distances = this.distToHurdle.getRange(null, previousStep, boardMaxDist); /* do we need a distLimitToHurdle to allow for variable hurdle distances (e.g. 2-4) */
        var minDistTo = distances[0]; var maxDistTo = distances[1];
        distances = this.distAfterHurdle.getRange(null, previousStep, boardMaxDist); /* doesn't the "board max" need recalculated based on hurdle position? */
        var minDistAfter = distances[0]; var maxDistAfter = distances[1];

        var hurdleCell = piece.position;
        for (var distTo = 1; distTo <= maxDistTo; distTo++) {
            hurdleCell = hurdleCell.links[dir];
            if (hurdleCell === undefined)
                break;

            var hurdle = hurdleCell.piece;
            if (hurdle == null)
                continue;

            if (distTo < minDistTo)
                break; // this piece is too close to hop over

            if (this.captureHurdle && !piece.canCapture(hurdle))
                break; // can't capture this hurdle, so can't hop it

            // now we've found the only possible hurdle. Do what we will with that, then there cannot be another hurdle, so break after
            var destCell = hurdleCell;
            for (var distAfter = 1; distAfter <= maxDistAfter; distAfter++) {
                destCell = destCell.links[dir];
                if (destCell === undefined)
                    break;

                var target = destCell.piece;

                if (distAfter >= minDistAfter) {
                    var captureStep = null;
                    if (this.moveWhen == MoveDefinition.When.Capture) {
                        if (target == null)
                            continue; // needs to be a capture for this slide to be valid, and there is no piece here. But there might be pieces beyond this one.
                        else if (piece.canCapture(target))
                            captureStep = MoveStep.CreateCapture(target, destCell, piece.ownerPlayer, game.holdCapturedPieces);
                        else
                            break; // cannot capture this piece. Slides cannot pass over pieces, so there can be no more valid slides in this direction.
                    }
                    else if (this.moveWhen == MoveDefinition.When.Move && target != null)
                        break;
                    else if (target != null) {
                        if (piece.canCapture(target))
                            captureStep = MoveStep.CreateCapture(target, destCell, piece.ownerPlayer, game.holdCapturedPieces);
                        else
                            break; // cannot capture this piece (probably own it)
                    }

                    var move = baseMove.clone();

                    if (this.captureHurdle) {
                        var hurdleCaptureStep = MoveStep.CreateCapture(hurdle, hurdle.position, piece.ownerPlayer, game.holdCapturedPieces);
                        move.addStep(hurdleCaptureStep);
                        move.addPieceReference(hurdle, "hurdle");
                    }

                    if (captureStep != null) {
                        move.addStep(captureStep);
                        move.addPieceReference(target, 'target');
                    }

                    move.addStep(MoveStep.CreateMove(piece, piece.position, destCell, dir));

                    if (this.conditions.isSatisfied(move, game))
                        moves.push(move);
                }

                if (target != null)
                    break; // whether we capture it or not, can't pass a second piece
            }

            break;
        }
    }

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

Shoot.prototype.appendValidNextSteps = function (baseMove, piece, game, previousStep) {
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

MoveLike.prototype.appendValidNextSteps = function (baseMove, piece, game, previousStep) {
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

ReferencePiece.prototype.appendValidNextSteps = function (baseMove, piece, game, previousStep) {
    var moves = [];

    return moves;
};

ReferencePiece.parse = function (xmlNode) {
    var node = $(xmlNode);
    var name = node.attr("name");

    var dir = node.attr("dir");
    if (dir === undefined)
        dir = null;
    else if (game.board.directionGroups.hasOwnProperty(dir))
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

ArbitraryAttack.prototype.appendValidNextSteps = function (baseMove, piece, game, previousStep) {
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

MoveGroup.prototype.appendValidNextSteps = function (baseMove, piece, game, previousStep) {
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