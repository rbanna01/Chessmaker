$(function () {
    
});

var game;
function loadDefinition(xml) {
    xml = $(xml.firstChild);
    
    game = new Game();
    game.board = new Board(game);

    var defs = SVG('defs');
    var boardSVG = game.board.loadSVG(xml, defs);

    PieceType.parseAll(xml.children('pieces'), defs);
    Player.parseAll(xml, game, boardSVG);

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
    else if (hasClass(this, 'option')) {
        selectMove(this);
        clearSelection();
    }
    else {
        clearSelection();
        addClass($(this), 'selected');

        var cell = game.board.cells[this.getAttribute('id')];
        if (cell != null && cell.piece != null)
            game.board.showMoveOptions(cell.piece);
    }

    return false;
}

function clearSelection() {
    var paths = $('#render .selected');
    if (paths.length == 0)
        return;

    remClass(paths, 'selected');

    var paths = $('#render .option');
    if (paths.length == 0)
        return;

    remClass(paths, 'option');
}

function selectMove(clicked) {
    var pieceCell = game.board.getCellBySelector('.selected');
    var destCell = game.board.getCellByElement(clicked);

    if (pieceCell == null || destCell == null)
        return;

    var piece = pieceCell.piece;
    if (piece == null)
        return;

    game.board.selectMoveByCell(piece, destCell);
}