"use strict";

Conditions.ResolveComparison = function (type, val1, val2) {
    switch (type) {
        case Conditions.NumericComparison.Equals:
            return val1 == val2;
        case Conditions.NumericComparison.LessThan:
            return val1 < val2;
        case Conditions.NumericComparison.GreaterThan:
            return val1 > val2;
        case Conditions.NumericComparison.LessThanOrEquals:
            return val1 <= val2;
        case Conditions.NumericComparison.GreaterThanOrEquals:
            return val1 >= val2;
        default:
            return false;
    }
}

Conditions.prototype.isSatisfied = function (move) {
    switch (this.type) {
        case Conditions.GroupType.And:
            for (var i = 0; i < this.elements.length; i++)
                if (!this.elements[i].isSatisfied(move))
                    return false;
            return true;
        case Conditions.GroupType.Or:
            for (var i = 0; i < this.elements.length; i++)
                if (this.elements[i].isSatisfied(move))
                    return true;
            return false;
        case Conditions.GroupType.Nand:
            for (var i = 0; i < this.elements.length; i++)
                if (!this.elements[i].isSatisfied(move))
                    return true;
            return false;
        case Conditions.GroupType.Nor:
            for (var i = 0; i < this.elements.length; i++)
                if (this.elements[i].isSatisfied(move))
                    return false;
            return true;
        case Conditions.GroupType.Xor:
            var any = false;
            for (var i = 0; i < this.elements.length; i++)
                if (this.elements[i].isSatisfied(move))
                    if (any)
                        return false;
                    else
                        any = true;
            return any;
        default: // there are no other types!
            return false;
    }
};

function Conditions_Type(of, type) {
    this.of = of;
    this.type = type;
}

Conditions_Type.prototype.isSatisfied = function(move) {
    var other = move.getPieceByRef(this.of);
    if (other == null)
        throw "Piece reference not found: " + this.of;

    return other.pieceType.name == this.type;
};

function Conditions_Owner(of, relationship) {
    this.of = of;
    this.relationship = relationship;
}

Conditions_Owner.prototype.isSatisfied = function (move) {
    var other = move.getPieceByRef(this.of);
    if (other == null)
        throw "Piece reference not found: " + this.of;

    if (this.relationship == Player.Relationship.Any)
        return true;

    return this.relationship == move.player.getRelationship(other.ownerPlayer);
};

function Conditions_MoveNumber(of, number, comparison) {
    this.of = of;
    this.number = number;
    this.comparison = comparison;
}

Conditions_MoveNumber.prototype.isSatisfied = function (move) {
    var other = move.getPieceByRef(this.of);
    if (other == null)
        throw "Piece reference not found: " + this.of;

    return Conditions.ResolveComparison(this.comparison, other.moveNumber, this.number);
};

function Conditions_MaxDist(from, dir, number, comparison) {
    this.from = from;
    this.dir = dir;
    this.number = number;
    this.comparison = comparison;
}

Conditions_MaxDist.prototype.isSatisfied = function (move) {
    var other = move.getPieceByRef(this.from);
    if (other == null)
        throw "Piece reference not found: " + this.from;

    var previousStep = move.steps.length > 0 ? move.steps[move.steps.length - 1] : null;
    var dirs = move.player.resolveDirection(this.dir, previousStep != null && previousStep.direction != null ? previousStep.direction : move.player.forwardDir);

    for (var i = 0; i < dirs.length; i++) {
        var maxDist = move.prevState.game.board.getMaxDistance(other.position, dirs[i]);
        if (Conditions.ResolveComparison(this.comparison, maxDist, this.number))
            return true;
    }
    return false;
};

function Conditions_TurnsSinceLastMove(of, number, comparison) {
    this.of = of;
    this.number = number;
    this.comparison = comparison;
}

Conditions_TurnsSinceLastMove.prototype.isSatisfied = function (move) {
    var other = move.getPieceByRef(this.of);
    if (other == null)
        throw "Piece reference not found: " + this.of;

    return Conditions.ResolveComparison(this.comparison, move.prevState.moveNumber - other.lastMoveTurn, this.number);
};

function Conditions_Threatened(start, end, value) {
    this.start = start;
    this.end = end;
    this.value = value;
}

Conditions_Threatened.alreadyChecking = false; // when performing moves to check this, don't go performing other moves for other "threatened" checks, or things get messy

Conditions_Threatened.prototype.isSatisfied = function (move) {
    if (Conditions_Threatened.alreadyChecking)
        return true;

    Conditions_Threatened.alreadyChecking = true;
    var retVal = this.checkSatisfied(move);
    Conditions_Threatened.alreadyChecking = false;

    return retVal;
};

Conditions_Threatened.prototype.checkSatisfied = function (move) {
    var step = move.steps[move.steps.length - 1]; // all steps except the current one will already have been performed

    if (this.start && step.fromState == Piece.State.OnBoard) {
        var threatened = Conditions_Threatened.isThreatened(move.prevState, step.fromPos);
        
        if (threatened != this.value)
            return false;
    }

    if (this.end && step.toState == Piece.State.OnBoard) {
        if (!step.perform(move.prevState.game, false))
            return false;

        var threatened = Conditions_Threatened.isThreatened(move.prevState, step.toPos);
        step.reverse(move.prevState.game, false);

        if (threatened != this.value)
            return false;
    }

    return true;
};

Conditions_Threatened.isThreatened = function (state, pos) {

    var moves = state.determineThreatMoves();

    for (var i = 0; i < moves.length; i++)
        if (moves[i].wouldCapture(pos))
            return true;

    return false;
};