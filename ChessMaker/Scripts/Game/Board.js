"use strict";

function Board(game) {
    this.game = game;
    this.elementID = 'render';
    this.cells = {};
    this.relativeDirections = {};
    this.directionGroups = {};
}

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