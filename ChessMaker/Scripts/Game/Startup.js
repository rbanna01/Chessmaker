$(function () {
    
});

var board;
function loadDefinition(xml) {
    xml = $(xml.firstChild);
    
    board = new Board();

    var defs = SVG('defs');
    var boardSVG = board.loadSVG(xml, defs);

    PieceType.parseAll(xml.children('pieces'), defs);
    Player.parseAll(xml, board, boardSVG);

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

function cellClicked(e) {
    if (hasClass(this, 'selected'))
        remClass($(this), 'selected');
    else {
        clearSelection();
        addClass($(this), 'selected');
        logCellInfo(this);
    }

    return false;
}

function clearSelection() {
    var paths = $('#render .selected');
    if (paths.length == 0)
        return;

    remClass(paths, 'selected');
}

function logCellInfo(cellNode) {
    var id = cellNode.getAttribute('id');

    var cell = board.cells[id];
    if (cell == null)
        return;

    if (cell.piece == null)
        console.log('Clicked ' + cell.name + ', which is empty');
    else
        console.log('Clicked ' + cell.piece.ownerPlayer.name + ' ' + cell.piece.pieceType.name + ' at ' + cell.name);

    var links = 'This cell links to';
    for (var link in cell.links) {
        links += ' ' + cell.links[link].name + ' (' + link + '),';
    }

    if (links == 'This cell links to')
        links += ' no other cells';
    console.log(links);
}