﻿"use strict";

function Conditions(type) {
    this.type = type;
    this.elements = [];
}

Conditions.GroupType = {
    And: 0,
    Or: 1,
    Nand: 2,
    Nor: 3,
    Xor: 4
};

Conditions.NumericComparison = {
    Equals: 0,
    LessThan: 1,
    GreaterThan: 2,
    LessThanOrEquals: 3,
    GreaterThanOrEquals: 4,
}

Conditions.NumericComparison.parse = function (str) {
    switch (str) {
        case 'equals':
            return Conditions.NumericComparison.Equals;
        case 'less than':
            return Conditions.NumericComparison.LessThan;
        case 'greater than':
            return Conditions.NumericComparison.GreaterThan;
        case 'less than or equals':
            return Conditions.NumericComparison.LessThanOrEquals;
        case 'greater than or equals':
            return Conditions.NumericComparison.GreaterThanOrEquals;
        default:
            throw 'Invalid comparison type: ' + str;
    }
};

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

Conditions.findNode = function (node) {
    var nodes = node.childNodes;
    for (var i = 0; i < nodes.length; i++) {
        var node = nodes[i];
        if (node.tagName == 'conditions') 
            return node;
    }
    return null;
}

Conditions.parse = function (node, type) {
    if (type === undefined)
        type = Conditions.GroupType.And;

    var group = new Conditions(type);
    if (node == null)
        return group;

    var children = node.childNodes;
    for (var i = 0; i < children.length; i++) {
        var child = children[i];
        var name = child.nodeName;

        switch (name) {
            case 'and':
                group.elements.push(Conditions.parse(child, Conditions.GroupType.And));
                break;
            case 'or':
                group.elements.push(Conditions.parse(child, Conditions.GroupType.Or));
                break;
            case 'nand':
                group.elements.push(Conditions.parse(child, Conditions.GroupType.Nand));
                break;
            case 'nor':
            case 'not':
                group.elements.push(Conditions.parse(child, Conditions.GroupType.Nor));
                break;
            case 'xor':
                group.elements.push(Conditions.parse(child, Conditions.GroupType.Xor));
                break;

            case 'type':
                var of = child.getAttribute('of');
                var type = child.textContent;
                group.elements.push(new Conditions_Type(of, type));
                break;
            case 'owner':
                var of = child.getAttribute('of');
                var relationship = Player.Relationship.parse(child.textContent);
                group.elements.push(new Conditions_Owner(of, relationship));
                break;
            case 'moveNumber':
                var of = child.getAttribute('of');
                if (of == null)
                    of = 'self';
                var number = parseInt(child.textContent);
                var comparison = Conditions.NumericComparison.parse(child.getAttribute('comparison'));
                group.elements.push(new Conditions_MoveNumber(of, number, comparison));
                break;
            case 'maxDist':
                var from = child.getAttribute('from');
                if (from == null)
                    from = 'self';

                var dir = child.getAttribute('dir');
                var number = parseInt(child.textContent);
                var comparison = Conditions.NumericComparison.parse(child.getAttribute('comparison'));
                group.elements.push(new Conditions_MaxDist(from, dir, number, comparison));
                break;

            case 'turnsSinceLastMove':
                var of = child.getAttribute('of');
                if (of == null)
                    of = 'self';
                var number = parseInt(child.textContent);
                var comparison = Conditions.NumericComparison.parse(child.getAttribute('comparison'));
                group.elements.push(new Conditions_TurnsSinceLastMove(of, number, comparison));
                break;

            case 'threatened':
                var value = child.textContent == 'true';
                var where = child.getAttribute('where');
                var start = where != 'end';
                var end = where != 'start';
                group.elements.push(new Conditions_Threatened(start, end, value));
                break;

            case 'num_pieces_in_range':
            case 'move_causes_check':
            case 'move_causes_checkmate':
            case 'checkmate':
            case 'pieces_threatened':
            case 'repeated_check':
            case 'no_moves_possible':
            case 'repetition_of_position':
            case 'turns_since_last_capture':
            case 'turns_since_last_move':
            default:
                console.log("Unrecognised condition type: " + name);
                break;
        }
    }

    return group;
};

Conditions.prototype.isSatisfied = function (move, state) {
    switch (this.type) {
        case Conditions.GroupType.And:
            for (var i = 0; i < this.elements.length; i++)
                if (!this.elements[i].isSatisfied(move, state))
                    return false;
            return true;
        case Conditions.GroupType.Or:
            for (var i = 0; i < this.elements.length; i++)
                if (this.elements[i].isSatisfied(move, state))
                    return true;
            return false;
        case Conditions.GroupType.Nand:
            for (var i = 0; i < this.elements.length; i++)
                if (!this.elements[i].isSatisfied(move, state))
                    return true;
            return false;
        case Conditions.GroupType.Nor:
            for (var i = 0; i < this.elements.length; i++)
                if (this.elements[i].isSatisfied(move, state))
                    return false;
            return true;
        case Conditions.GroupType.Xor:
            var any = false;
            for (var i = 0; i < this.elements.length; i++)
                if (this.elements[i].isSatisfied(move, state))
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

Conditions_Type.prototype.isSatisfied = function(move, state) {
    var other = move.getPieceByRef(this.of);
    if (other == null)
        throw "Piece reference not found: " + this.of;

    return other.pieceType.name == this.type;
};

function Conditions_Owner(of, relationship) {
    this.of = of;
    this.relationship = relationship;
}

Conditions_Owner.prototype.isSatisfied = function (move, state) {
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

Conditions_MoveNumber.prototype.isSatisfied = function (move, state) {
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

Conditions_MaxDist.prototype.isSatisfied = function (move, state) {
    var other = move.getPieceByRef(this.from);
    if (other == null)
        throw "Piece reference not found: " + this.from;

    var previousStep = move.steps.length > 0 ? move.steps[move.steps.length - 1] : null;
    var dirs = move.player.resolveDirection(this.dir, previousStep != null && previousStep.direction != null ? previousStep.direction : move.player.forwardDir);

    for (var i = 0; i < dirs.length; i++) {
        var maxDist = state.game.board.getMaxDistance(other.position, dirs[i]);
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

Conditions_TurnsSinceLastMove.prototype.isSatisfied = function (move, state) {
    var other = move.getPieceByRef(this.of);
    if (other == null)
        throw "Piece reference not found: " + this.of;

    return Conditions.ResolveComparison(this.comparison, state.moveNumber - other.lastMoveTurn, this.number);
};

function Conditions_Threatened(start, end, value) {
    this.start = start;
    this.end = end;
    this.value = value;
}

Conditions_Threatened.alreadyChecking = false; // when performing moves to check this, don't go performing other moves for other "threatened" checks, or things get messy

Conditions_Threatened.prototype.isSatisfied = function (move, state) {
    if (Conditions_Threatened.alreadyChecking)
        return true;

    Conditions_Threatened.alreadyChecking = true;
    var retVal = this.checkSatisfied(move, state);
    Conditions_Threatened.alreadyChecking = false;

    return retVal;
};

Conditions_Threatened.prototype.checkSatisfied = function (move, state) {
    var step = move.steps[move.steps.length - 1]; // all steps except the current one will already have been performed

    if (this.start && step.fromState == Piece.State.OnBoard) {
        var prevStep = move.steps.length > 1 ? move.steps[move.steps.length - 2] : null;
        var threatened = this.isThreatened(move, prevStep, state, step.fromPos);
        
        if (threatened != this.value)
            return false;
    }

    if (this.end && step.toState == Piece.State.OnBoard) {
        if (!step.perform(state.game, false))
            return false;

        var threatened = this.isThreatened(move, step, state, step.toPos);
        step.reverse(state.game, false);

        if (threatened != this.value)
            return false;
    }

    return true;
};

Conditions_Threatened.prototype.isThreatened = function (move, step, state, pos) {

    var moves = state.determineThreatMoves(move.player, step);

    for (var i = 0; i < moves.length; i++)
        if (moves[i].wouldCapture(pos))
            return true;

    return false;
};