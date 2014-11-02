$(function () {
    
});

function loadDefinition(xml) {
    xml = $(xml.firstChild);
    var boardSVG = loadBoard(xml);
    loadInitialLayout(xml, boardSVG);

    $('#main').append(boardSVG);

    $('#render path.cell').click(cellClicked);
    /*
    $('#main').click(function (e) {
        if (!e.shiftKey && !e.ctrlKey)
            clearSelection();
    });
    */
    calculateRatio();
    resizeBoard();
    $(window).resize(function () { resizeBoard(); });
}

function loadBoard(xml) {
    var boardSVG = SVG('svg');

    var boardXml = xml.children('board');

    boardSVG.setAttribute('xmlns', 'http://www.w3.org/2000/svg');
    boardSVG.setAttribute('id', 'render');
    boardSVG.setAttribute('viewBox', boardXml.attr('viewBox'));
    
    var defs = SVG('defs');
    boardSVG.appendChild(defs);

    // create cells & board lines
    boardXml.children().each(function () {
        if (this.tagName == 'cell') {
            var cell = SVG('path');
            var cellID = this.getAttribute('id');
            cell.setAttribute('id', cellID);
            var path = this.getAttribute('path');
            cell.setAttribute('d', path);

            var cssClass = 'cell ' + this.getAttribute('fill');
            var border = this.getAttribute('border');
            if ( border != null)
                cssClass += ' ' + border + 'Stroke';
            cell.setAttribute('class', cssClass);

            boardSVG.appendChild(cell);

            // save off each cell's position
            var seg = cell.pathSegList.getItem(0);
            cellCoordX[cellID] = seg.x;
            cellCoordY[cellID] = seg.y;
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

    // add defs entry for each piece type & appearance
    var piecesXml = xml.children('pieces');
    piecesXml.children().each(function () {
        var pieceName = this.getAttribute('name');
        $(this).children('appearance').each(function () {
            var appearanceName = this.getAttribute('player');

            var def = SVG('g');
            def.setAttribute('id', pieceName + '_' + appearanceName)
            def.setAttribute('width', this.getAttribute('width'));
            def.setAttribute('height', this.getAttribute('height'));
            def.innerHTML = this.innerHTML;
            defs.appendChild(def);
        });
    });

    return boardSVG;
}

function loadInitialLayout(xml, boardSVG) {
    var setupXml = xml.children('setup');
    setupXml.children().each(function () {
        var playerName = this.getAttribute('name');
        $(this).children('piece').each(function () {
            var type = this.getAttribute('type');
            var location = this.getAttribute('location');

            var xPos = cellCoordX[location];
            var yPos = cellCoordY[location];

            if (xPos == undefined || yPos == undefined) {
                console.log('Unrecognised piece location: ' + location);
                return;
            }

            var piece = SVG('use');
            piece.setAttribute('x', xPos-22);
            piece.setAttribute('y', yPos-22);
            piece.setAttributeNS("http://www.w3.org/1999/xlink", 'href', '#' + type + '_' + playerName);

            boardSVG.appendChild(piece);
        })
    });
}

function cellClicked(e) {
    if (hasClass(this, 'selected'))
        remClass($(this), 'selected');
    else {
        clearSelection();
        addClass($(this), 'selected');
    }

    return false;
}

function clearSelection() {
    var paths = $('#render .selected');
    if (paths.length == 0)
        return;

    remClass(paths, 'selected');
}
