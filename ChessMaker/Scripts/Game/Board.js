﻿function Board(game) {
    this.game = game;
    this.svgID = 'render';
    this.cells = {};
    this.relativeDirections = {};
    this.directionGroups = {};
    this.allDirections = [];
}

Board.prototype.loadSVG = function(xml, defs) {
    var boardSVG = SVG('svg');

    var boardXml = xml.children('board');

    boardSVG.setAttribute('xmlns', 'http://www.w3.org/2000/svg');
    boardSVG.setAttribute('id', this.svgID);
    boardSVG.setAttribute('viewBox', boardXml.attr('viewBox'));
    
    boardSVG.appendChild(defs);

    // create cells & board lines
    var board = this;
    boardXml.children().each(function () {
        if (this.tagName == 'cell') {
            var cell = new Cell(this.getAttribute('id'), $(this).children('link'));
            board.cells[cell.name] = cell;
            boardSVG.appendChild(cell.loadSVG(this));
        }
        else if (this.tagName == 'line') {
            var line = SVG('line');
            line.setAttribute('x1', this.getAttribute('x1'));
            line.setAttribute('x2', this.getAttribute('x2'));
            line.setAttribute('y1', this.getAttribute('y1'));
            line.setAttribute('y2', this.getAttribute('y2'));
            line.setAttribute('class', 'detail ' + this.getAttribute('color') + 'Stroke');
            boardSVG.appendChild(line);
        }
    });

    var allDirs = {};

    for (var sourceRef in this.cells) {
        var cell = this.cells[sourceRef];
        for (var dir in cell.links) {
            if (!allDirs.hasOwnProperty(dir))
                allDirs[dir] = dir;

            var ref = cell.links[dir];

            if (this.cells.hasOwnProperty(ref))
                cell.links[dir] = this.cells[ref];
            else
                throw 'Cell ' + cell.name + ' has link to unrecognised cell: ' + ref;
        }
    }

    for (var dir in allDirs)
        this.allDirections.push(dir);

    return boardSVG;
}

Board.prototype.parseDirections = function (dirsNode) {
    var board = this;
    dirsNode.children('relative').each(function () {
        var dirNode = $(this);
        var name = dirNode.attr('name');
        var relDir = {};

        dirNode.children().each(function () {
            var link = $(this);
            relDir[link.attr('from')] = link.attr('to');
        });

        board.relativeDirections[name] = relDir;
    });

    dirsNode.children('group').each(function () {
        var groupNode = $(this);
        var name = groupNode.attr('name');
        var group = [];

        groupNode.children().each(function () {
            var link = $(this);
            group.push(link.attr('dir'));
        });

        board.directionGroups[name] = group;
    });
};

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