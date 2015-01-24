"use strict";

function Board(game) {
    this.game = game;
    this.elementID = 'render';
    this.cells = {};
    this.relativeDirections = {};
    this.directionGroups = {};
    this.allDirections = [];
}

// whether its a global, relative or group name, this should output an array of resultant global directions
Board.prototype.resolveDirection = function (name, prevDir) {
    if (name == 'same')
        return [prevDir];
    else if (name == 'any')
        return this.allDirections;

    if (this.directionGroups.hasOwnProperty(name)) {
        var group = this.directionGroups[name];
        var output = [];
        for (var i = 0; i < group.length; i++) {
            var dir = this.resolveSingleDirection(group[i], prevDir);
            if (dir != null)
                output.push(dir);
        }
        return output;
    }

    var dir = this.resolveSingleDirection(name, prevDir);
    if (dir == null)
        return [];
    return [dir];
};

Board.prototype.resolveSingleDirection = function (name, prevDir) {
    if (this.relativeDirections.hasOwnProperty(name)) {
        var relDir = this.relativeDirections[name];
        if (relDir.hasOwnProperty(prevDir))
            return relDir[prevDir];
        else
            return null;
    }

    return name;
};

Board.prototype.getMaxDistance = function (cell, dir) {
    var num = 0;

    var alreadyVisited = {};
    alreadyVisited[cell.name] = cell;

    while (cell.links.hasOwnProperty(dir)) {
        cell = cell.links[dir];

        // without this bit, circular boards would infinite loop
        if (alreadyVisited.hasOwnProperty(cell.name))
            break;
        alreadyVisited[cell.name] = cell;

        num++;
    }

    return num;
};

Board.prototype.getCellBySelector = function (selector) {
    var cellImage = $('#render path.cell' + selector);
    if (cellImage.length == 0)
        return null;

    return this.getCellByElement(cellImage[0]);
};

Board.prototype.getCellByElement = function (element) {
    var cell = game.board.cells[element.getAttribute('id')];
    if (cell === undefined)
        return null;
    return cell;
};

Board.prototype.updatePiecePositions = function (groupNode) {
    /*
    this ought to sort by value, rather than just by image (really just type, alphabetically)
    */
    var pieces = $(groupNode).children().sort(function(a, b) {
        a = a.getAttributeNS('http://www.w3.org/1999/xlink', 'href');
        b = b.getAttributeNS('http://www.w3.org/1999/xlink', 'href');
        return a > b ? 1 : -1;
    });

    var counts = {}, players = 0;
    pieces.each(function () {
        var player = this.getAttribute('class');

        if (counts.hasOwnProperty(player))
            counts[player]++;
        else {
            counts[player] = 1;
            players++;
        }
    });

    var fullSpacedHeight = 48;
    
    var biggestCount = 1;
    for (var player in counts)
        biggestCount = Math.max(biggestCount, counts[player]);

    /*
    if players is sufficently low, multiple columns per player would be nice to have
    */

    var stepSize = Math.min((groupNode.clientHeight - fullSpacedHeight) / biggestCount, fullSpacedHeight);

    counts = {};
    pieces.each(function () {
        var player = this.getAttribute('class');

        var num;
        if (counts.hasOwnProperty(player))
            num = ++ counts[player];
        else
            num = counts[player] = 1;

        this.setAttribute('y', (num - 0.45) * stepSize);
    });
};