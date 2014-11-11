﻿function Board(game) {
    this.game = game;
    this.svgID = 'render';
    this.cells = {};
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

    for (var sourceRef in this.cells) {
        var cell = this.cells[sourceRef];
        for (var dir in cell.links) {
            var ref = cell.links[dir];

            if (this.cells.hasOwnProperty(ref))
                cell.links[dir] = this.cells[ref];
            else
                throw 'Cell ' + cell.name + ' has link to unrecognised cell: ' + ref;
        }
    }

    return boardSVG;
}

Board.prototype.showMoveOptions = function (piece) {
    var moves = piece.getPossibleMoves(this.game);
    for (var i = 0; i < moves.length; i++) {
        var destCell = moves[i].getEndPos();
        var img = destCell.getImage();
        addClassSingle(img, 'option');
    }
};

Board.prototype.selectMoveByCell = function (piece, cell) {
    if (piece.ownerPlayer == null)
        console.log('piece.ownerPlayer is null');
    if (piece.ownerPlayer === undefined)
        console.log('piece.ownerPlayer is undefined');

    var moves = piece.getPossibleMoves(this.game);
    for (var i = 0; i < moves.length; i++) {
        var move = moves[i]
        var destCell = move.getEndPos();
        if (destCell != cell)
            continue;

        if (move.perform(game, true))
            game.endTurn();
        else
            console.log('unable to perform move');
        break;
    }
};

Board.prototype.getCellBySelector = function (selector) {
    var cellImage = $('#render path.cell' + selector);
    if (cellImage.length == 0)
        return null;

    return this.getCellByElement(cellImage[0]);
}

Board.prototype.getCellByElement = function (element) {
    var cell = game.board.cells[element.getAttribute('id')];
    if (cell === undefined)
        return null;
    return cell;
}

function Cell(name, links) {
    this.name = name;
    this.coordX = 0;
    this.coordY = 0;
    this.piece = null;
    this.links = {};

    var cell = this;
    links.each(function () {
        var dir = this.getAttribute('dir');
        var ref = this.getAttribute('to');

        cell.links[dir] = ref;
    });
}

Cell.prototype.loadSVG = function (xml) {
    var cell = SVG('path');
    cell.setAttribute('id', this.name);
    var path = xml.getAttribute('path');
    cell.setAttribute('d', path);

    var cssClass = 'cell ' + xml.getAttribute('fill');
    var border = xml.getAttribute('border');
    if (border != null)
        cssClass += ' ' + border + 'Stroke';
    cell.setAttribute('class', cssClass);

    // save off each cell's position
    var seg = cell.pathSegList.getItem(0);
    this.coordX = seg.x;
    this.coordY = seg.y;
    return cell;
}


Cell.prototype.getImage = function () {
    return document.getElementById(this.name);
}