$(function () {
    
});

function loadDefinition(xml) {
    xml = $(xml.firstChild);
    createBoard(xml);
    
    // load pieces etc
}

function createBoard(xml) {
    var boardXml = xml.children('board');

    var board = SVG('svg');
    board.setAttribute('xmlns', 'http://www.w3.org/2000/svg');
    board.setAttribute('id', 'render');
    board.setAttribute('viewBox', boardXml.attr('viewBox'));
    
    // create each cell
    boardXml.children().each(function () {
        if (this.tagName == 'cell') {
            var cell = SVG('path');
            cell.setAttribute('id', this.getAttribute('id'))
            cell.setAttribute('d', this.getAttribute('path'));

            var cssClass = 'cell ' + this.getAttribute('fill');
            var border = this.getAttribute('border');
            if ( border != null)
                cssClass += ' ' + border + 'Stroke';
            cell.setAttribute('class', cssClass);

            board.appendChild(cell);
        }
        else if (this.tagName == 'line') {
            var line = SVG('line');
            line.setAttribute('x1', this.getAttribute('x1'));
            line.setAttribute('x2', this.getAttribute('x2'));
            line.setAttribute('y1', this.getAttribute('y1'));
            line.setAttribute('y2', this.getAttribute('y2'));
            line.setAttribute('class', 'detail ' + this.getAttribute('color') + 'Stroke');
            board.appendChild(line);
        }
    });

    $('#main').append(board);

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
